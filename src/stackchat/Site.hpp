#pragma once

#include "stackchat/rooms/Room.hpp"

#include <string>
#include <map>
#include <memory>

namespace stackchat {

struct Site {
    std::string fkey;

    int uid;
    std::string username;

    std::map<unsigned int, std::shared_ptr<Room>> rooms;
};

}
