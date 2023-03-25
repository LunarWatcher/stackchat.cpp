#include "stackchat/StackChat.hpp"

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
        .userAgent = "StackChatCppDemos/git (+https://github.com/LunarWatcher/stackchat.cpp)"
    });

    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
}

