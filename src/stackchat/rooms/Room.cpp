#include "Room.hpp"

#include "cpr/cpr.h"
#include "cpr/cprtypes.h"
#include "cpr/verbose.h"
#include "nlohmann/json.hpp"
#include "stackchat/StackChat.hpp"
// Temporary
#include <iostream>

namespace stackchat {

Room::Room(StackChat* chat, StackSite site, unsigned int rid) : chat(chat), site(site), rid(rid) {
    auto& siteInfo = chat->sites.at(site);

    webSocket.setUrl(getWSUrl(siteInfo.fkey));
    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            std::cout << "received message: " << msg->str << std::endl;
            std::cout << "> " << std::flush;
        }
    });

}

std::string Room::getWSUrl(const std::string& fkey) {
    //auto timeReq = cpr::Post(
        //cpr::Verbose(),
        //cpr::Body{
            //nlohmann::json{
                //{"fkey", fkey}
            //}.dump()
        //},
        //cpr::Header{{"Referer", "https://chat.stackoverflow.com/rooms/1/sandbox"}},
        //chat->cookies, chat->conf.userAgent,
        //cpr::Url(fmt::format("https://chat.{}/chats/{}/events", siteUrlMap[site], rid))
    //);

    //if (timeReq.status_code == 500) {
        //throw std::runtime_error("Stack failed to return a timestamp: internal error (details unknown)");
    //}

    //std::string time = std::to_string(nlohmann::json::parse(
            //timeReq.text
    //).at("time").get<long long>());

    auto wsUrlReq = cpr::Post(
        cpr::Verbose{},
        cpr::Url {fmt::format("https://chat.{}/ws-auth", siteUrlMap[site])},
        //cpr::Payload {
            //{"fkey", fkey},
            //{"roomid", std::to_string(rid)}
        //}, 
        cpr::Body{"fkey=" + fkey + "&roomid=" + std::to_string(rid)},


        chat->cookies, chat->conf.userAgent);
    
    if (wsUrlReq.status_code == 404) {
        throw std::runtime_error("Received 404: library bug or Stack is struggling: " + wsUrlReq.text);
    } else {
        throw std::runtime_error("Unexpected server-sided error: " + wsUrlReq.text);
    }
    return nlohmann::json::parse(wsUrlReq.text)
        .at("url").get<std::string>() + "?l=999999999999999";
}

}
