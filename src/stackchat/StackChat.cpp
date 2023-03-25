#include "StackChat.hpp"
#include "stackchat/rooms/StackSite.hpp"

#include <regex>
#include <nlohmann/json.hpp>
#include <ixwebsocket/IXNetSystem.h>
#include <iostream>

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
}

void StackChat::join(StackSite site, unsigned int rid) {
    if (!sites.contains(site)) {
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

void StackChat::setCookies(const cpr::Response &res, StackSite site) {
    auto& s = sites[site];
    auto& cookies = s.cookies;
    auto& authSess = s.authSess;

    for (auto& cookie : res.cookies) {
        cookies.push_back(cookie);
    }
    authSess.setCookies(cookies);
}

}
