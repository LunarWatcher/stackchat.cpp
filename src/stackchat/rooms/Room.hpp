#pragma once

#include "cpr/payload.h"
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
    bool intentionalShutdown = false;

    std::chrono::system_clock::time_point lastSocketMessage = std::chrono::system_clock::now();

    std::vector<long long> performSendMessage(
        std::optional<ChatEvent> replyEvent,
        const std::string& content,
        MessageType type,
        MessageLengthPolicy lengthPolicy);

    std::string getWSUrl(const std::string& fkey);
public:

    enum class AccessLevel {
        NONE,
        READ,
        READWRITE,
        OWNER
    };
    static inline std::map<AccessLevel, std::string> accessToString {
        {AccessLevel::NONE, "remove"},
        {AccessLevel::READ, "read"},
        {AccessLevel::READWRITE, "read-write"},
        {AccessLevel::OWNER, "owner"},
    };


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

    void leaveRoom();

    /**
     * Checks if the chatroom socket needs to be restarted.
     * Should only be called by the daemon in StackChat.
     */
    void checkRevive();

    bool setUserAccess(AccessLevel level, long long userId);

    /**
     * Utility for adding an fkey to a payload
     */
    cpr::Payload fkeyed(cpr::Payload p);
};

}
