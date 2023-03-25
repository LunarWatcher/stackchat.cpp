#pragma once

#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cpr/cpr.h>

#include "cpr/cookies.h"
#include "cpr/response.h"
#include "stackchat/Site.hpp"
#include "stackchat/rooms/StackSite.hpp"
#include "stackchat/web/MTSession.hpp"

namespace stackchat {

struct ChatConfig {
    std::string email;
    std::string password;

    std::string prefix = "";
    cpr::UserAgent userAgent = "StackChatCppUnannouncedUser/git";

};

class StackChat {
private:
    static inline auto logger = spdlog::stdout_color_mt("StackChat");
public:
    std::map<StackSite, Site> sites;
    ChatConfig conf;
    std::atomic<bool> isRunning{true};

    StackChat(const ChatConfig& conf);

    void login(StackSite site);
    void join(StackSite site, unsigned int rid);

    void reloadFKey(StackSite site);
    void setCookies(const cpr::Response& res, StackSite site);

    std::string chatUrl(StackSite site) {
        return fmt::format("https://chat.{}", siteUrlMap.at(site));
    }

    void block() {
        while (isRunning.load()) {
            // TODO: this is nasty. Fix
            std::this_thread::sleep_for(std::chrono::minutes(10));
        }
    }
};

}
