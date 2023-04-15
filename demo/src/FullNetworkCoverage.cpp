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
            .prefix = "~", // Required for command functionality. Defaults to nothing, which will disable commands
            .userAgent = "StackChatCppDemos/git (+https://github.com/LunarWatcher/stackchat.cpp)",
    });

    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
    chat.join(stackchat::StackSite::STACKEXCHANGE, 144870);
    chat.join(stackchat::StackSite::META_STACKEXCHANGE, 1696);

    chat.block();
}

