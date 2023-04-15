#pragma once

#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cpr/cpr.h>

#include "cpr/cookies.h"
#include "cpr/response.h"
#include "stackchat/Site.hpp"
#include "stackchat/chat/Command.hpp"
#include "stackchat/rooms/Room.hpp"
#include "stackchat/rooms/StackSite.hpp"
#include "stackchat/web/MTSession.hpp"
#include "chat/ChatEvent.hpp"
#include <functional>

namespace stackchat {

struct ChatConfig {
    std::string email;
    std::string password;

    std::string prefix = "";
    cpr::UserAgent userAgent = "StackChatCppUnannouncedUser/git";

    bool ignoreSelf = true;

};

using EventCallback = std::function<void(Room&, const ChatEvent&)>;

class StackChat {
private:
    static inline auto logger = spdlog::stdout_color_mt("StackChat");

    std::map<ChatEvent::Code, std::vector<EventCallback>> eventListeners;
    std::map<std::string, std::shared_ptr<Command>> commandCallbacks;
public:
    std::map<StackSite, Site> sites;

    ChatConfig conf;
    std::atomic<bool> isRunning{true};

    StackChat(const ChatConfig& conf);

    void login(StackSite site);
    void join(StackSite site, unsigned int rid);

    void reloadFKey(StackSite site);
    void setCookies(const cpr::Response& res, StackSite site);

    void sendTo(StackSite site, unsigned int rid, const std::string& content);

    void registerCommand(std::string commandName, std::shared_ptr<Command> cmd);
    void registerEventListener(ChatEvent::Code ev, EventCallback func);

    void broadcast(Room& r, ChatEvent& ev);

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
