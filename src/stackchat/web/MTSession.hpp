#pragma once

#include <cpr/cpr.h>

namespace stackchat {

class MTSession {
public:
    std::mutex m;
    cpr::Session sess;

    MTSession() {
        sess.SetAcceptEncoding({cpr::AcceptEncodingMethods::gzip, cpr::AcceptEncodingMethods::zlib, cpr::AcceptEncodingMethods::deflate});
    }

    template <typename... Ts>
    cpr::Response Get(Ts&&... ts) {
        wipeParams();
        std::lock_guard<std::mutex> lock(m);
        cpr::priv::set_option(sess, std::forward<Ts>(ts)...);
        
        auto res = sess.Get();
        return res;
    }

    template <typename... Ts>
    cpr::Response Post(Ts&&... ts) {
        wipeParams();
        std::lock_guard<std::mutex> lock(m);
        cpr::priv::set_option(sess, std::forward<Ts>(ts)...);
        
        auto res = sess.Post();
        return res;
    }

    void setCookies(cpr::Cookies& cookies);
    void wipeParams();
};

}
