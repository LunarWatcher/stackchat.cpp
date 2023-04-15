#include "StackChat.hpp"
#include "stackchat/rooms/StackSite.hpp"

#include <regex>
#include <nlohmann/json.hpp>
#include <ixwebsocket/IXNetSystem.h>
#include <iostream>
#include <stc/StringUtil.hpp>

namespace stackchat {

StackChat::StackChat(const ChatConfig& conf) : conf(conf) {
    if (conf.email.size() == 0 || conf.password.size() == 0) {
        throw std::runtime_error("Misconfiguration: empty email and/or password");
    }

    ix::initNetSystem();
}


void StackChat::login(StackSite site) {
    auto loginSite = site == StackSite::STACKEXCHANGE ? StackSite::META_STACKEXCHANGE : site;
    auto baseURL = siteUrlMap[loginSite];

    cpr::Url loginURL = fmt::format("https://{}/users/login", baseURL);
    auto& authSess = sites[site].authSess;

    auto fkeyReq = authSess.Get(loginURL, cpr::Parameters {{"returnurl", "/"}});
    setCookies(fkeyReq, site);

    if (fkeyReq.status_code != 200) {
        throw std::runtime_error("Request failed (" + std::to_string(fkeyReq.status_code) + "): " + fkeyReq.text + "; " + fkeyReq.error.message);
    }

    // Blast from the past
    const static std::regex fkeyFinder("<input.*name=\"fkey\" +(?:type=\"hidden\" +)?value=\"(.*?)\"", std::regex_constants::icase);

    std::smatch match;
    if (!std::regex_search(fkeyReq.text, match, fkeyFinder)) {
        throw std::runtime_error("Failed to locate main fkey; has Stack changed their HTML again?");
    }

    std::string fkey = match.str(1);
    if (fkey.size() == 0) {
        throw std::runtime_error("Failed to extract fkey");
    }

    cpr::Payload payload = {
        {"email", conf.email},
        {"password", conf.password},
        {"fkey", fkey}
    };
    auto loginReq = authSess.Post(loginURL, payload);
    setCookies(loginReq, site);

    if (loginReq.status_code != 200) {
        throw std::runtime_error("Login request failed (" + std::to_string(fkeyReq.status_code) + "): " + fkeyReq.text + "; " + fkeyReq.error.message);
    }
    // Stupid system: check cookies to make sure we've actually logged in
    bool good = false;
    for (auto& cookie : loginReq.cookies) {
        if (cookie.GetName() == "acct") {
            good = true;
            break;
        }
    }
    if (!good || loginReq.url != fmt::format("https://{}/", siteUrlMap[loginSite])) {
        throw std::runtime_error("Failed to log in: " + loginReq.url.str() + " (if the URL talks about captcha, you need to log in manually via your browser, or wait a significant amount of time before trying again)");
    }

    reloadFKey(site);
    logger->info("Successfully logged in to {}", siteUrlMap[site]);

    logger->info("Scraping UID and username...");
    // This is exceptionally lazy, but admittedly and in my biased opinion, bigbrain as fuck
    // By requesting a bogus page, the regex used later is guaranteed to only find the currently logged-in user, because there are no other
    // links that can interfere with the process.
    auto frontPageGetRes = authSess.Get(cpr::Url("https://chat." + siteUrlMap[site] + "/fake-page-for-uid--stackchat-dot-cpp--login"));
    setCookies(frontPageGetRes, site);
    

    const static std::regex userUIDScrape("<a href=\"/users/(\\d+)/.*?\" ", std::regex_constants::icase | std::regex_constants::optimize);
    if (!std::regex_search(frontPageGetRes.text, match, userUIDScrape)) {
        logger->error("Request target: {}", frontPageGetRes.url.str());
        throw std::runtime_error("Failed to locate the UID. Has Stack's HTML changed again? :(");
    }

    std::string stringifiedUid = match[1];
    sites[site].uid = std::stoi(stringifiedUid);

    logger->info("Successfully found UID ({}). Scraping username...", stringifiedUid);

    auto userPage = authSess.Get(cpr::Url("https://chat." + siteUrlMap[site] + "/users/" + match[1].str()));
    if (userPage.status_code >= 400) {
        logger->error("Link URL source: {}", frontPageGetRes.url.str());
        throw std::runtime_error("Chat user page errored out in spite of existing. Has merge fuckery been involved?");
    }

    const static std::regex usernameScrape("<h1>(.*)</h1>", std::regex_constants::icase | std::regex_constants::optimize);
    if (!std::regex_search(userPage.text, match, usernameScrape)) {
        throw std::runtime_error("Failed to locate the username. Has Stack's HTML changed again? :(");
    }

    std::string username = match[1];
    logger->info("Bot account IDed as {} ({})", username, stringifiedUid);

    sites[site].username = username;

    
}

void StackChat::join(StackSite site, unsigned int rid) {
    if (!sites[site].fkey.size()) {
        login(site);
    }
    // TODO: check if joined already

    std::shared_ptr<Room> room = std::make_shared<Room>(this, site, rid);
    sites.at(site).rooms[rid] = room;
}

void StackChat::reloadFKey(StackSite site) {
    const static std::regex chatFkeySelector("<input.*id=\"fkey\".*value=\"(.*?)\"", std::regex_constants::icase);

    auto fkeyReq = sites[site].authSess.Get(cpr::Url{chatUrl(site)});
    setCookies(fkeyReq, site);

    std::smatch match;
    if (!std::regex_search(fkeyReq.text, match, chatFkeySelector)) {
        throw std::runtime_error("Failed to locate chat fkey; has Stack changed their HTML again?");
    }

    std::string fkey = match.str(1);
    if (fkey.size() == 0) {
        throw std::runtime_error("Failed to extract fkey");
    }


    sites[site].fkey = fkey;
}

void StackChat::sendTo(StackSite site, unsigned int rid, const std::string& content) {
    auto& siteInfo = sites[site];
    auto& rooms = siteInfo.rooms;
    if (!rooms.contains(rid) || rooms.at(rid) == nullptr) {
        throw std::runtime_error("Room not joined.");
    }

    rooms.at(rid)->sendMessage(content);
}


void StackChat::setCookies(const cpr::Response &res, StackSite site) {
    auto& s = sites[site];
    auto& cookies = s.cookies;
    auto& authSess = s.authSess;

    for (auto& cookie : res.cookies) {
        cookies.push_back(cookie);
    }
    authSess.setCookies(cookies);
}

void StackChat::registerCommand(std::string commandName, std::shared_ptr<Command> func) {
    commandCallbacks[commandName] = func;
}
void StackChat::registerEventListener(ChatEvent::Code ev, EventCallback func) {
    eventListeners[ev].push_back(func);
}
// This is horribly named. Fucking hell
void StackChat::broadcast(Room& r, ChatEvent &ev) {
    if (conf.prefix != "" && (ev.type == ChatEvent::Code::EDIT || ev.type == ChatEvent::Code::NEW_MESSAGE)) {
        auto& content = ev.messageEvent.content;

        // TODO: Complex command structures (maybe not here, could maybe do a new class. Not sure, TBD)
        if (content.starts_with(conf.prefix)) {
            auto stripPrefix = content.substr(conf.prefix.size());
            if (stripPrefix.size() != 0) {
                std::vector<std::string> split = stc::string::split(stripPrefix, ' ', 1);
                auto cmd = split[0];
                std::string arg = split.size() == 1 ? "" : split[1];

                if (commandCallbacks.contains(cmd)) {
                    commandCallbacks.at(cmd)->onMessage(
                        r,
                        ev,
                        stc::string::split(arg, " ")
                    );
                    return;
                }
            }
        }
    }
    for (auto& listener : eventListeners[ev.type]) {
        listener(r, ev);
    }
}

}
