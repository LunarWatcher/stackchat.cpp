#pragma once

#include "cpr/cpr.h"
#include <memory>
#include "stackchat/rooms/StackSite.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/format.h>

namespace stackchat {

class StackChat;
class Browser {
private:
    static inline auto logger = spdlog::stdout_color_mt("Browser");
public:
    StackChat* chat;

    std::mutex m;
    cpr::Session sess;
    cpr::Cookies c{false};

    Browser(StackChat* c);

    template <typename... Ts>
    cpr::Response Get(Ts&&... ts) {
        std::lock_guard<std::mutex> lock(m);
        cpr::priv::set_option(sess, std::forward<Ts>(ts)...);
        
        auto res = sess.Get();
        for (auto& cookie : res.cookies) {
            c.push_back(cookie);
        }
        sess.SetCookies(c);
        return res;
    }

    template <typename... Ts>
    cpr::Response Post(Ts&&... ts) {
        std::lock_guard<std::mutex> lock(m);
        cpr::priv::set_option(sess, std::forward<Ts>(ts)...);
        
        auto res = sess.Post();
        for (auto& cookie : res.cookies) {
            c.push_back(cookie);
        }
        sess.SetCookies(c);
        return res;
    }

    std::string chatUrl(StackSite site) {
        return fmt::format("https://chat.{}", siteUrlMap.at(site));
    }

    void login(StackSite site);
    void join(StackSite site, unsigned int rid);

    void reloadFKey(StackSite site);

};

}
