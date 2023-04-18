#pragma once

#include "spdlog/logger.h"
#include "stackchat/rooms/StackSite.hpp"
#include "stackchat/web/MTSession.hpp"
#include <ixwebsocket/IXWebSocket.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "stackchat/chat/ChatEvent.hpp"

namespace stackchat {

enum class MessageType {
    NORMAL,
    QUOTE,
    CODE
};

enum class MessageLengthPolicy {
    BREAK,
    INSERT_NEWLINE,
    FORCE_CODE,
    NONE
};

class StackChat;
class Room {
private:
    std::shared_ptr<spdlog::logger> logger;

    long long performSendMessage(
        std::optional<ChatEvent> replyEvent,
        const std::string& content,
        MessageType type,
        MessageLengthPolicy lengthPolicy);
public:
    StackChat* chat;
    MTSession sess;

    const StackSite site;
    const unsigned int rid;

    ix::WebSocket webSocket;

    Room(StackChat* chat, StackSite site, unsigned int rid);

    long long sendMessage(
        const std::string& content,
        MessageType type = MessageType::NORMAL,
        MessageLengthPolicy lengthPolicy = MessageLengthPolicy::BREAK);
    long long reply(
        const ChatEvent& ev, const std::string& content, MessageType type = MessageType::NORMAL,
        MessageLengthPolicy lengthPolicy = MessageLengthPolicy::INSERT_NEWLINE
    );

    std::string getWSUrl(const std::string& fkey);
};

}
