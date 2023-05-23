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

#include "stackchat/rooms/StackSite.hpp"
#include "stc/StringUtil.hpp"

namespace stackchat {

Room::Room(StackChat* chat, StackSite site, unsigned int rid) : chat(chat), site(site), rid(rid) {
    logger = spdlog::get("room-" + siteUrlMap[site] + "-" + std::to_string(rid));
    if (logger == nullptr) {
        logger = spdlog::stdout_color_mt("room-" + siteUrlMap[site] + "-" + std::to_string(rid));
    }
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
                this->chat->sendToListeners(*this, evObj);
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

std::vector<long long> Room::performSendMessage(std::optional<ChatEvent> replyEvent,
                                   const std::string& rawContent,
                                   MessageType type,
                                   MessageLengthPolicy lengthPolicy) {
    if (rawContent.size() == 0) {
        logger->error("Cannot send an empty message");
        return {-1};
    }
    int count = 0;
    std::string content;

    switch (type) {
    case MessageType::NORMAL: {
        content = (replyEvent.has_value() ? ":" + std::to_string(replyEvent->messageEvent.message_id) + " " : "") + rawContent;

        if (content.size() > 500
                && content.find('\n') == std::string::npos) {
            switch (lengthPolicy) {
            case MessageLengthPolicy::BREAK: {
                std::string c1 = content.substr(0, 500);
                std::string c2 = content.substr(500);

                auto retVal = performSendMessage(std::nullopt, c1, type, lengthPolicy);
                // recursively send new messages
                auto additionalIDs = performSendMessage(std::nullopt, c2, type, lengthPolicy);
                retVal.insert(retVal.end(), additionalIDs.begin(), additionalIDs.end());
                return retVal;
                } break;
            case MessageLengthPolicy::NONE:
                break;
            case MessageLengthPolicy::INSERT_NEWLINE:
                content += "\n.";
                break;
            case MessageLengthPolicy::FORCE_CODE:
                return performSendMessage(std::nullopt, rawContent + "\n.", MessageType::CODE, lengthPolicy);
            }
        }
        } break;
    case MessageType::CODE: {
        auto lines = stc::string::split(rawContent, "\n");
        for (auto& line : lines) {
            content += "    " + line + "\n";
        }
        if (content.size() > 500 && content.find('\n') == std::string::npos) {
            if (lengthPolicy == MessageLengthPolicy::INSERT_NEWLINE) {
                content += "\n.";
            } else {
                throw std::runtime_error("Message too long, and not set to auto-break.");
            }
        }
        
        } break;
    case MessageType::QUOTE:
        content = (replyEvent.has_value() ? ":" + std::to_string(replyEvent->messageEvent.message_id) + " " : "") + "> " + rawContent;

        if (content.size() > 500 && content.find('\n') == std::string::npos) {
            switch (lengthPolicy) {
            case MessageLengthPolicy::BREAK: {
                std::string c1 = content.substr(0, 500);
                std::string c2 = content.substr(500);
                
                std::vector<long long> retVal = performSendMessage(replyEvent, c1, type, lengthPolicy);
                auto additionalIDs = performSendMessage(std::nullopt, c2, type, lengthPolicy);
                retVal.insert(retVal.end(), additionalIDs.begin(), additionalIDs.end());
                return retVal;
                } break;
            case MessageLengthPolicy::FORCE_CODE:
                throw std::runtime_error("Unsupported for this message type.");
            case MessageLengthPolicy::INSERT_NEWLINE:
                content += "\n.";
                break;
            default:
                // Silence the compiler
                break;
            }
        }
        break;
    }

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
            return {-1};
        } else if (res.text.find("This room has been frozen") != std::string::npos) {
            logger->error("Room {} ({}) is frozen", rid, siteUrlMap[site]);
            return {-1};
        } else if (res.text.find("The room does not exist") != std::string::npos) {
            logger->error("Room {} ({}) doesn't exist", rid, siteUrlMap[site]);
            return {-1};
        } else if (res.status_code != 200) {
            logger->error("Stack errorred out for \"{}\" (HTTP {})", content, res.status_code);
            return {-1};
        } else {
            logger->info("Sent {} to {} ({})", content, rid, siteUrlMap[site]);
            try {
                auto id = nlohmann::json::parse(res.text).value<long long>("id", -1);
                return {id};
            } catch (...) {
                logger->error(res.text);
                throw;
            }
        }

        count++;
    } while (count < 5);
    return {-1};
}

std::vector<long long> Room::sendMessage(const std::string& rawContent, MessageType type, MessageLengthPolicy lengthPolicy) {
    return performSendMessage(std::nullopt, rawContent, type, lengthPolicy);
}

std::vector<long long> Room::reply(const ChatEvent& ev, const std::string& rawContent, MessageType type, MessageLengthPolicy lengthPolicy) {

    return performSendMessage(ev, rawContent, type, lengthPolicy);
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
    auto timeJson = nlohmann::json::parse(
            timeReq.text
    );

    if (!timeJson.contains("time")) {
        throw std::runtime_error(fmt::format("Failed to find time. Received response: {}", timeReq.text));
    }
    
    std::string time = std::to_string(timeJson.at("time").get<long long>());

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
