#pragma once

#include "fmt/format.h"
#include <stdexcept>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

namespace stackchat {

/**
 * Defines a StackSite.
 *
 * Note that this can be used with fmt::format to get out the full URL for the site. The following
 * format identifiers are supported:
 * * {:c}: URL to the chat site (excluding https)
 * * {:#}: Full URL to the site (including https)
 * * {:#c}: Full URL to the chat site (including https)
 *
 * If none are supplied, the main domain is returned.
 */
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
    bool chatLink = false;
    bool fullLink = false;

    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin();
        if (it == ctx.end()) {
            return it;
        }
        if (*it == '#') {
            fullLink = true;
            ++it;
        }
        if (*it == 'c') {
            chatLink = true;
            ++it;
        }

        if (it != ctx.end() && *it != '}') {
            throw std::runtime_error("Invalid format specifier");
        }

        return it;
    }

    constexpr auto format(const stackchat::StackSite& site, format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "{}{}{}", 
            fullLink ? "https://" : "",
            chatLink ? "chat." : "",
            stackchat::siteUrlMap.at(site)
        );
    };
};
