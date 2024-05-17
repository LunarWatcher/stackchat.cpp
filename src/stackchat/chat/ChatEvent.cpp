#include "ChatEvent.hpp"

namespace stackchat {

bool ChatEvent::isAccessRequest() const {
    return type == Code::USER_ACCESS_CHANGED
        && messageEvent.content == CONTENT_ACCESS_REQUEST;
}

}
