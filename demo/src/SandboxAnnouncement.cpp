#include "stackchat/StackChat.hpp"
#include "stackchat/chat/ChatEvent.hpp"
#include "stackchat/chat/Command.hpp"

#include <iostream>
#include <string>

// This is not the only way to define a command, for the record. Utility classes exist (or will be added when necessary),
// but the goal is to be as flexible as possible
class InterjectCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) {
        r.reply(ev, "I'd like to interject for a moment. What you're referring to as chat login is, in fact, an eternal source of pain and suffering.");
    }
};

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

    chat.registerCommand("interject", std::make_shared<InterjectCommand>());

    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
    chat.sendTo(stackchat::StackSite::STACKOVERFLOW, 1, "Chat login is horrible. *angry fox noises*");

    chat.block();
}

