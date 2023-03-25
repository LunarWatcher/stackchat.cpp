#pragma once

#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cpr/cpr.h>

#include "stackchat/Site.hpp"
#include "stackchat/rooms/StackSite.hpp"

namespace stackchat {

struct ChatConfig {
    std::string email;
    std::string password;

    std::string prefix = "";
    cpr::UserAgent userAgent = "StackChatUnannouncedUser/git";

};

class StackChat {
private:
    static inline auto logger = spdlog::stdout_color_mt("StackChat");


public:
    std::map<StackSite, Site> sites;
    cpr::Cookies cookies{{{"x", "0"}}, false};
    ChatConfig conf;

    StackChat(const ChatConfig& conf);

    void login(StackSite site);

    void join(StackSite site, unsigned int rid);
};

}
