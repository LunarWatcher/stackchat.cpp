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

    std::vector<long long> performSendMessage(
        std::optional<ChatEvent> replyEvent,
        const std::string& content,
        MessageType type,
        MessageLengthPolicy lengthPolicy);

    std::string getWSUrl(const std::string& fkey);
public:
    StackChat* chat;
    MTSession sess;

    const StackSite site;
    const unsigned int rid;

    ix::WebSocket webSocket;

    Room(StackChat* chat, StackSite site, unsigned int rid);

    /**
     * Sends a message to this room.
     */
    std::vector<long long> sendMessage(
        const std::string& content,
        MessageType type = MessageType::NORMAL,
        MessageLengthPolicy lengthPolicy = MessageLengthPolicy::BREAK);

    /**
     * Replies to a message in this room.
     *
     * Note that it isn't checked whether the message replied to is actually in the current room.
     * Sending replies in other rooms than the source room will make a non-functioning reply, likely
     * displaying as the raw `:message_id_here` format.
     */
    std::vector<long long> reply(
        const ChatEvent& ev, const std::string& content, MessageType type = MessageType::NORMAL,
        MessageLengthPolicy lengthPolicy = MessageLengthPolicy::INSERT_NEWLINE
    );

};

}
