#pragma once

#include "stackchat/rooms/Room.hpp"
#include "stackchat/web/MTSession.hpp"

#include <string>
#include <map>
#include <memory>

namespace stackchat {

struct Site {
    std::string fkey;

    int uid;
    std::string username;
    std::string pingUsername;

    std::map<unsigned int, std::shared_ptr<Room>> rooms;

    MTSession authSess;
    cpr::Cookies cookies{false};
};

}
