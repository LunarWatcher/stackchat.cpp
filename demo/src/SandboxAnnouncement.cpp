#include "stackchat/StackChat.hpp"
#include "stackchat/chat/ChatEvent.hpp"

#include <iostream>
#include <string>

int main() {
    std::string email = std::getenv("STACK_EMAIL"), password = std::getenv("STACK_PASSWORD");
    if (email == "" || password == "") {
        throw std::runtime_error("Please provide credentials to run the demo.");
    }

    stackchat::StackChat chat({
        .email = email,
        .password = password,
        .prefix = "~",
        .userAgent = "StackChatCppDemos/git (+https://github.com/LunarWatcher/stackchat.cpp)",
    });

    chat.registerCommand("interject", [](stackchat::Room& r, const stackchat::ChatEvent& ev, const std::string&) {
        r.sendMessage("I'd like to interject for a moment. What you're referring to as chat login is, in fact, an eternal source of pain and suffering.");
    });

    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
    chat.sendTo(stackchat::StackSite::STACKOVERFLOW, 1, "Chat login is horrible. *angry fox noises*");

    chat.block();
}

