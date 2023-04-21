#include "stackchat/web/MTSession.hpp"

namespace stackchat {

void MTSession::setCookies(cpr::Cookies &cookies) {
    sess.SetCookies(cookies);
}

void MTSession::wipeParams() {
    sess.SetParameters({});
    sess.SetPayload({});
}

}
