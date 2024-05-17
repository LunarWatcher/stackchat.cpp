#include "ChatEvent.hpp"
#include "stc/StringUtil.hpp"

namespace stackchat {

bool ChatEvent::isAccessRequest() const {
    return type == Code::USER_ACCESS_CHANGED
        && messageEvent.content == CONTENT_ACCESS_REQUEST;
}

std::string ChatEvent::getPing() const {
    std::string res = username;
    stc::string::replaceAll(res, " ", "");
    return "@" + res;
}

}
