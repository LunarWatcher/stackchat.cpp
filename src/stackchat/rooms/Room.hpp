#pragma once

#include "stackchat/rooms/StackSite.hpp"
#include "stackchat/web/MTSession.hpp"
#include <ixwebsocket/IXWebSocket.h>

namespace stackchat {

class StackChat;
class Room {
public:
    StackChat* chat;
    MTSession sess;

    const StackSite site;
    const unsigned int rid;

    ix::WebSocket webSocket;

    Room(StackChat* chat, StackSite site, unsigned int rid);

    std::string getWSUrl(const std::string& fkey);
};

}
