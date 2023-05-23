#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>

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

NLOHMANN_JSON_SERIALIZE_ENUM( StackSite, {
    {StackSite::STACKOVERFLOW, "stackoverflow"},
    {StackSite::STACKEXCHANGE, "stackexchange"},
    {StackSite::META_STACKEXCHANGE, "metastackexchange"},
})


}
