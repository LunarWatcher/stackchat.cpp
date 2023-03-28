#pragma once

#include "stackchat/rooms/Room.hpp"

namespace stackchat {

class Command {
public:
    virtual ~Command() = default;

    virtual void onMessage(
        Room& room,
        const ChatEvent& receivedMessage,
        const std::vector<std::string>& vArgs) = 0;

};

}
