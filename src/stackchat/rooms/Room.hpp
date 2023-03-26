#pragma once

#include "spdlog/logger.h"
#include "stackchat/rooms/StackSite.hpp"
#include "stackchat/web/MTSession.hpp"
#include <ixwebsocket/IXWebSocket.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "stackchat/chat/ChatEvent.hpp"

namespace stackchat {

class StackChat;
class Room {
private:
    std::shared_ptr<spdlog::logger> logger;
public:
    StackChat* chat;
    MTSession sess;

    const StackSite site;
    const unsigned int rid;

    ix::WebSocket webSocket;

    Room(StackChat* chat, StackSite site, unsigned int rid);

    void sendMessage(const std::string& content);
    void reply(ChatEvent& ev);

    std::string getWSUrl(const std::string& fkey);
};

}
