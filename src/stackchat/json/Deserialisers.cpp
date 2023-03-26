#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

#include "stackchat/chat/ChatEvent.hpp"

#define COND_AT(field, dest) if (j.contains(field)) { \
        j.at(field).get_to(dest); \
    }

namespace stackchat {

void from_json(const nlohmann::json& j, ChatEvent &ev) {
    try {
        ev.type = static_cast<ChatEvent::Code>(j.at("event_type").get<int>());
    } catch (...) {
        ev.type = ChatEvent::Code::UNKNOWN;
        spdlog::warn("Unknown event detected: {}", j.dump());
        return;
    }

    j.at("time_stamp").get_to(ev.timestamp);
    j.at("id").get_to(ev.event_id);


    COND_AT("room_id", ev.room_id);
    COND_AT("room_name", ev.room_name);

    COND_AT("user_id", ev.user_id);
    COND_AT("user_name", ev.username);

    COND_AT("message_id", ev.messageEvent.message_id);
    COND_AT("message_edits", ev.messageEvent.message_edits);
    COND_AT("content", ev.messageEvent.content);
    COND_AT("parent_id", ev.messageEvent.parent_id);
    COND_AT("target_user_id", ev.messageEvent.target_user_id);
    COND_AT("show_parent", ev.messageEvent.show_parent);
    COND_AT("message_stars", ev.messageEvent.message_stars);
    COND_AT("message_owner_stars", ev.messageEvent.message_owner_stars);

}

}
