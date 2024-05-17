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

    /**
     * Controls whether or not to enable a thread that handles quiet websocket death, an annoying quirk of chat.
     * Quiet websocket death happens because chat is beta software that hasn't been touched in a decade. The
     * consequence is that no data makes it through the websocket, but the socket still remains active.
     * It's probably somehow left in a semi-detached state on Stack's side.
     * Anyway, because the socket doesn't actually die, error management from socket disconnection can't kick in.
     * 
     * To deal with this, StackChat has a built-in quiet death recovery system. 
     *
     * You probably only want to set this to false un unit tests or if
     * doing other obscure shit.
     */
    bool reviveRunner = true;

};

using EventCallback = std::function<void(Room&, const ChatEvent&)>;

class StackChat {
private:
    static inline auto logger = spdlog::stdout_color_mt("StackChat");

    std::map<ChatEvent::Code, std::vector<EventCallback>> eventListeners;
    std::map<std::string, std::shared_ptr<Command>> commandCallbacks;

    std::thread recoveryRunner;

    void recoverDeadSockets();
public:
    std::map<StackSite, Site> sites;

    ChatConfig conf;
    std::atomic<bool> isRunning{true};

    StackChat(const ChatConfig& conf);
    StackChat(const StackChat& src);
    StackChat(StackChat&& src);
    ~StackChat();

    void login(StackSite site);
    void join(StackSite site, unsigned int rid);

    void reloadFKey(StackSite site);
    void setCookies(const cpr::Response& res, StackSite site);

    void sendTo(StackSite site, unsigned int rid, const std::string& content);

    void registerCommand(std::string commandName, std::shared_ptr<Command> cmd);
    void registerEventListener(ChatEvent::Code ev, EventCallback func);

    void sendToListeners(Room& r, ChatEvent& ev);

    /**
     * Gets the fkey for a given StackSite.
     *
     * Note that if you're not logged in, this function will throw. Do not use unless you've logged into the site
     * first.
     */
    std::string getFkey(StackSite site);

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
