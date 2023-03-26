#include "Room.hpp"

#include "cpr/cpr.h"
#include "cpr/cprtypes.h"
#include "cpr/verbose.h"
#include "fmt/core.h"
#include "nlohmann/json.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "stackchat/StackChat.hpp"
#include "stackchat/chat/ChatEvent.hpp"
// Temporary
#include <iostream>

namespace stackchat {

Room::Room(StackChat* chat, StackSite site, unsigned int rid) : chat(chat), site(site), rid(rid) {
    logger = spdlog::stdout_color_mt("room-" + std::to_string(rid));
    auto& siteInfo = chat->sites.at(site);

    sess.setCookies(siteInfo.cookies);
    webSocket.setUrl(getWSUrl(siteInfo.fkey));
    webSocket.setExtraHeaders({{"Origin", chat->chatUrl(site)}});
    webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            nlohmann::json ev = nlohmann::json::parse(msg->str);
            if (!ev.contains("r" + std::to_string(this->rid))) {
                return;
            }
            auto roomEvents = ev["r" + std::to_string(this->rid)];
            if (!roomEvents.contains("e")) {
                return;
            }

            for (auto& event : roomEvents.at("e")) {
                ChatEvent evObj = event.get<ChatEvent>();
                this->chat->broadcast(*this, evObj);
            }

        } else if (msg->type == ix::WebSocketMessageType::Error) {
            logger->error("Error occurred: {}", msg->errorInfo.reason);
        }
    });
    webSocket.setMinWaitBetweenReconnectionRetries(20000); // Note: in millis
    // TODO: figure out if I have to disable automatic reconnects or not.
    // Really hinges on whether the URL remains valid or not. Could probably check what chatexchange does
    webSocket.start();

    logger->info("Listening to room.");
}

void Room::sendMessage(const std::string& content) {
    int count = 0;
    do {
        auto res = sess.Post(
            cpr::Url{fmt::format("{}/chats/{}/messages/new", chat->chatUrl(site), rid)},
            cpr::Payload{
                {"fkey", chat->sites[site].fkey},
                {"text", content}
            }
        );
        if (res.text.find("You can perform this action again in") != std::string::npos) {
            int inPos = res.text.find(" in ") + 3;
            int limit = std::stoi(res.text.substr(inPos));

            logger->warn("Rate limited for {} seconds in {} ({})", limit, rid, siteUrlMap[site]);
            // Add one second for good measure. Stack's rounding is notoriously bad
            std::this_thread::sleep_for(std::chrono::seconds(limit + 1));
        } else if (res.text.find("You need 20 reputation points") != std::string::npos) {
            logger->error("Not enough rep to post {} ({})", rid, siteUrlMap[site]);
            return;
        } else if (res.text.find("This room has been frozen") != std::string::npos) {
            logger->error("Room {} ({}) is frozen", rid, siteUrlMap[site]);
            return;
        } else if (res.text.find("The room does not exist") != std::string::npos) {
            logger->error("Room {} ({}) doesn't exist", rid, siteUrlMap[site]);
            return;
        } else {
            logger->info("Sent {} to {} ({})", content, rid, siteUrlMap[site]);
            return;
        }

        count++;
    } while (count < 5);
}

std::string Room::getWSUrl(const std::string& fkey) {
    auto timeReq = sess.Post(
        cpr::Payload{
            {"since", "0"},
            {"mode", "Messages"},
            {"msgCount", "100"},
            {"fkey", fkey}
        },
        chat->conf.userAgent,
        cpr::Url(fmt::format("https://chat.{}/chats/{}/events", siteUrlMap[site], rid))
    );

    if (timeReq.status_code == 500) {
        throw std::runtime_error("Stack failed to return a timestamp: internal error (details unknown)");
    }

    std::string time = std::to_string(nlohmann::json::parse(
            timeReq.text
    ).at("time").get<long long>());

    auto wsUrlReq = sess.Post(
        cpr::Url {fmt::format("https://chat.{}/ws-auth", siteUrlMap[site])},
        cpr::Payload {
            {"fkey", fkey},
            {"roomid", std::to_string(rid)}
        }, 
        chat->conf.userAgent);
    
    if (wsUrlReq.status_code == 404) {
        throw std::runtime_error("Received 404: library bug or Stack is struggling: " + wsUrlReq.text);
    } else if (wsUrlReq.status_code != 200 && wsUrlReq.status_code != 302){
        throw std::runtime_error("Unexpected server-sided error: " + wsUrlReq.text);
    }
    return nlohmann::json::parse(wsUrlReq.text)
        .at("url").get<std::string>() + "?l=" + time;
}


}
