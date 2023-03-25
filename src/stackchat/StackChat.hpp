#pragma once

#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cpr/cpr.h>

#include "stackchat/Site.hpp"
#include "stackchat/rooms/StackSite.hpp"
#include "stackchat/web/Browser.hpp"

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
    Browser br;
    ChatConfig conf;

    StackChat(const ChatConfig& conf);

    void login(StackSite site);
    void join(StackSite site, unsigned int rid);


};

}
