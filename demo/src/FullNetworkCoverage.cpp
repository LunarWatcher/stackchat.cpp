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

    // Connecting to multiple sites in the network is trivial. Simply provide the site and the room ID, and the API
    // library takes care of the rest.
    //
    // Login is automatically handled, if deemed necessary, when connecting to a room. This also helps simplify the code.
    // Note that the bot needs to have an account on all sites it tries to join, or the join function will throw one of a number
    // of exceptions that can occur as a result of a perceived bad login, or failed data acquisition.
    //
    // TL;DR: joining sites you don't have an account on will throw, but as long as you have the rep and accounts in order,
    // everything will be fine.
    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
    chat.join(stackchat::StackSite::STACKEXCHANGE, 144870);
    chat.join(stackchat::StackSite::META_STACKEXCHANGE, 1696);

    chat.block();
}

