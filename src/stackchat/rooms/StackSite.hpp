#pragma once

#include "fmt/format.h"
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

template <>
struct fmt::formatter<stackchat::StackSite> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    constexpr auto format(const stackchat::StackSite& site, format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", stackchat::siteUrlMap.at(site));
    };
};
