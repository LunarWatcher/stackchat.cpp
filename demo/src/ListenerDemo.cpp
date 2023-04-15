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

    // Listeners are registered with an event and a callback
    // Unlike commands, they don't (currently) use classes, as there isn't as much of a need for them.
    // Command structures have secondary uses, while listeners generally don't show up on most help systems.
    // Essentially, there's not really an advantage to making these a class, aside potentially allowing code reuse
    // (in the edge-case of a listener wanting to take NEW_MESSAGE and EDIT), though this in particular is a problem
    // for future me :)
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

