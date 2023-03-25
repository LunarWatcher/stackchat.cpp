#pragma once

#include "stackchat/rooms/StackSite.hpp"
#include <ixwebsocket/IXWebSocket.h>

namespace stackchat {

class StackChat;
class Room {
public:
    const StackChat* chat;
    const StackSite site;
    const unsigned int rid;
    ix::WebSocket webSocket;

    Room(StackChat* chat, StackSite site, unsigned int rid);

    std::string getWSUrl(const std::string& fkey);
};

}
