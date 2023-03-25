#include "Browser.hpp"

#include <regex>
#include <fmt/format.h>

#include "stackchat/StackChat.hpp"
#include "stackchat/rooms/Room.hpp"

namespace stackchat {

Browser::Browser(StackChat* chat) : chat(chat) {
    sess.SetUserAgent(chat->conf.userAgent);
}

void Browser::login(StackSite site) {
    auto baseURL = siteUrlMap[site];

    cpr::Url loginURL = fmt::format("https://{}/users/login", baseURL);

    auto fkeyReq = Get(loginURL, cpr::Parameters {{"returnurl", "/"}});

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
        {"email", chat->conf.email},
        {"password", chat->conf.password},
        {"fkey", fkey}
    };
    auto loginReq = Post(loginURL, payload);


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
    if (!good || loginReq.url != fmt::format("https://{}/", siteUrlMap[site])) {
        throw std::runtime_error("Failed to log in: " + loginReq.url.str() + " (if the URL talks about captcha, you need to log in manually via your browser, or wait a significant amount of time before trying again)");
    }

    reloadFKey(site);
    logger->info("Successfully logged in to {}", siteUrlMap[site]);
}

void Browser::reloadFKey(StackSite site) {
    const static std::regex chatFkeySelector("<input.*id=\"fkey\".*value=\"(.*?)\"", std::regex_constants::icase);

    auto fkeyReq = Get(cpr::Url{chatUrl(site)});

    std::smatch match;
    if (!std::regex_search(fkeyReq.text, match, chatFkeySelector)) {
        throw std::runtime_error("Failed to locate chat fkey; has Stack changed their HTML again?");
    }

    std::string fkey = match.str(1);
    if (fkey.size() == 0) {
        throw std::runtime_error("Failed to extract fkey");
    }


    chat->sites[site].fkey = fkey;
}

void Browser::join(StackSite site, unsigned int rid) {
    if (!chat->sites.contains(site)) {
        login(site);
    }
    // TODO: check if joined already

    std::shared_ptr<Room> room = std::make_shared<Room>(chat, site, rid);
    chat->sites.at(site).rooms[rid] = room;

}

}
