#pragma once

#include "nlohmann/json.hpp"
#include <string>

namespace stackchat {

struct ChatMessageEvent {
    std::string content;
    long long message_id;
    long long message_edits;
    long long message_stars;
    long long message_owner_stars;
    long long target_user_id;
    long long parent_id;

    bool show_parent; // Used for replies I think?
};

struct ChatEvent {
public:
    static inline std::string CONTENT_ACCESS_REQUEST = "Access now request";

    enum class Code {
        /**
         * Internal control
         */
        UNKNOWN = -1,
        /**
         * Internal control variable; used to forward all events.
         */
        ALL = 0,

        // See https://github.com/jbis9051/JamesSOBot/blob/master/docs/CHAT_API.md#events-we-care-about
        // for descriptions of the rest of these.
        NEW_MESSAGE = 1,
        EDIT = 2,
        USER_JOIN = 3,
        USER_LEAVE = 4,
        ROOM_INFO_CHANGED = 5,
        STARS_CHANGED = 6,
        DEBUG_MESSAGE = 7,
        PING = 8,
        MESSAGE_FLAGGED = 9,
        MESSAGE_DELETED = 10,
        FILE_ADDED = 11,
        MOD_FLAG = 12,
        USER_IGNORED_OR_UNIGNORED = 13,
        GLOBAL_NOTIFICATION = 14,
        USER_ACCESS_CHANGED = 15,
        USER_NOTIFICAITON = 16,
        INVITATION = 17,
        REPLY = 18,
        MESSAGE_MOVED_OUT = 19,
        MESSAGE_MOVED_IN = 20,
        TIMEOUT = 21,
        TICKER_UPDATE = 22,
        USER_SUSPENDED = 29,
        ACCOUNTS_MERGED = 30,
        USER_UPDATED = 34,

    };

    Code type;
    long long timestamp;

    long long event_id;
    long long user_id;
    std::string username;
    long long room_id;
    std::string room_name;

    /**
     * Contains some message-specific events.
     *
     * Note that some or all the fields may not be populated depending on the type of event.
     */
    ChatMessageEvent messageEvent;

    /**
     * Internal control variable; indicates whether or not the message was processed as a command.
     * This is only set to true if ev.type == ChatEvent::Code::EDIT or NEW_MESSAGE,
     * and it starts with the prefix.
     *
     * Note that @mentions result in two events (PING and NEW_MESSAGE (or EDIT)),
     * and _might_ result in 3 if it's also a reply
     *
     */
    bool processedAsCommand = false;

    bool isAccessRequest() const;
    /**
     * Returns the username without spaces. The result of this function includes an @ for pinging,
     * so there's no need to prefix one.
     *
     * Note that this function only returns a usable result of the username field is well-defined
     * for the event.
     */
    std::string getPing() const;
};

extern void from_json(const nlohmann::json& j, ChatEvent& ev);

}
