#include "StackChat.hpp"

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
    auto baseURL = siteUrlMap[site];

    cpr::Url loginURL = fmt::format("https://{}/users/login", baseURL);

    auto fkeyReq = cpr::Get(loginURL, cookies, cpr::Parameters {{"returnurl", "/"}}, conf.userAgent);
    // I _think_ this is fine. Assumes cookies sent to the server are always returned.
    for (auto cookie : fkeyReq.cookies) {
        cookies.push_back(cookie);
    }
    if (fkeyReq.status_code != 200) {
        throw std::runtime_error("Request failed (" + std::to_string(fkeyReq.status_code) + "): " + fkeyReq.text + "; " + fkeyReq.error.message);
    }

    // Blast from the past
    std::regex fkeyFinder("<input.*name=\"fkey\" +(?:type=\"hidden\" +)?value=\"(.*?)\"", std::regex_constants::icase);

    std::smatch match;
    if (!std::regex_search(fkeyReq.text, match, fkeyFinder)) {
        throw std::runtime_error("Failed to locate fkey; has Stack changed their HTML again?");
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
    auto loginReq = cpr::Post(loginURL, cookies, payload, conf.userAgent);
    for (auto cookie : loginReq.cookies) {
        if (cookie.GetName() == "uauth" || cookie.GetName() == "prov_tgt") continue;
        cookies.push_back(cookie);
    }

    if (loginReq.status_code != 200) {
        throw std::runtime_error("Login request failed (" + std::to_string(fkeyReq.status_code) + "): " + fkeyReq.text + "; " + fkeyReq.error.message);
    }
    // Stupid system: check cookies to make sure we've actually logged in
    bool good = false;
    for (auto& cookie : cookies) {
        if (cookie.GetName() == "acct") {
            good = true;
            break;
        }
    }
    if (!good || loginReq.url != fmt::format("https://{}/", siteUrlMap[site])) {
        throw std::runtime_error("Failed to log in: " + loginReq.url.str() + " (if the URL talks about captcha, you need to log in manually via your browser, or wait a significant amount of time before trying again)");
    }

    auto chatReq = cpr::Get(cpr::Url{fmt::format("https://chat.{}/", baseURL)}, cookies, conf.userAgent);
    for (auto cookie : chatReq.cookies) {
        if (cookie.GetName() == "uauth" || cookie.GetName() == "prov_tgt") continue;
        cookies.push_back(cookie);
    }



    sites[site] = {
        fkey
    };
    
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

}
