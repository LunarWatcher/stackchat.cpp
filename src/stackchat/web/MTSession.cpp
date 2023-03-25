#pragma once

#include "stackchat/web/MTSession.hpp"
namespace stackchat {

void MTSession::setCookies(cpr::Cookies &cookies) {
    sess.SetCookies(cookies);
}

}
