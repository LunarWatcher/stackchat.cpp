#pragma once

#include <string>
#include <map>

namespace stackchat {

enum class StackSite {
    STACKOVERFLOW,
    STACKEXCHANGE,
    META_STACKEXCHANGE
};

inline std::map<StackSite, std::string> siteUrlMap {
    {StackSite::STACKOVERFLOW, "stackoverflow.com"},
    {StackSite::STACKEXCHANGE, "stackexchange.com"},
    {StackSite::META_STACKEXCHANGE, "meta.stackexchange.com"}
};

}
