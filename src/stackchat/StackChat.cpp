#include "StackChat.hpp"

#include <regex>
#include <nlohmann/json.hpp>
#include <ixwebsocket/IXNetSystem.h>
#include <iostream>

namespace stackchat {

StackChat::StackChat(const ChatConfig& conf) : conf(conf), br(this) {
    if (conf.email.size() == 0 || conf.password.size() == 0) {
        throw std::runtime_error("Misconfiguration: empty email and/or password");
    }

    ix::initNetSystem();
}


void StackChat::login(StackSite site) {
    br.login(site);
}
void StackChat::join(StackSite site, unsigned int rid) {
    br.join(site, rid);
}

}
