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

    chat.registerEventListener(stackchat::ChatEvent::Code::USER_JOIN, [](stackchat::Room& room, const stackchat::ChatEvent& ev) {
        room.sendMessage("Hello, " + ev.username);
    });
    chat.registerEventListener(stackchat::ChatEvent::Code::USER_LEAVE, [](stackchat::Room& room, const stackchat::ChatEvent& ev) {
        room.sendMessage("Goodbye, " + ev.username);
    });
    chat.registerEventListener(stackchat::ChatEvent::Code::NEW_MESSAGE, [](stackchat::Room& room, const stackchat::ChatEvent& ev) {
        room.sendMessage("> " + ev.messageEvent.content);
    });
    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);

    chat.block();
}

