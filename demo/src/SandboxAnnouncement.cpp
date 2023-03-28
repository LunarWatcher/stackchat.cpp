#include "stackchat/StackChat.hpp"
#include "stackchat/chat/ChatEvent.hpp"
#include "stackchat/chat/Command.hpp"

#include <iostream>
#include <string>

// This is not the only way to define a command, for the record. Utility classes exist (or will be added when necessary),
// but the goal is to be as flexible as possible.
//
// The system does take a heavy OOP approach, largely to allow for context-based commands. I also like OOP, so that's part of it.
// Plus, it comes with the advantage of allowing stuff like MultiLevelCommand, used in a different demo.
class InterjectCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) {
        r.reply(ev, "I'd like to interject for a moment. What you're referring to as chat login is, in fact, an eternal source of pain and suffering.");
    }
};

int main() {
    // This isn't a fantastic way to get the credentials, but it is an option. There are other options as well,
    // this is just the laziest approach I can think of.
    std::string email = std::getenv("STACK_EMAIL"), password = std::getenv("STACK_PASSWORD");
    if (email == "" || password == "") {
        // This is deceptively useless; missing credentials actually results in an initialisation exception (something something null string)
        throw std::runtime_error("Please provide credentials to run the demo.");
    }

    stackchat::StackChat chat({
        .email = email,
        .password = password,
        .prefix = "~", // Required for command functionality. Defaults to nothing, which will disable commands
        .userAgent = "StackChatCppDemos/git (+https://github.com/LunarWatcher/stackchat.cpp)",
    });

    chat.registerCommand("interject", std::make_shared<InterjectCommand>());

    // There are many ways to deal with chat joining, as well as announcements and config, but this is left as an exercise to the reader, because there
    // are very, very many ways to implement it, depending on what the bot is used for.
    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
    // As an aside, this isn't really the only way to do join announcements.
    // It would be possible to hook up a join listener, and check if the bot triggered the event.
    chat.sendTo(stackchat::StackSite::STACKOVERFLOW, 1, "Chat login is horrible. *angry fox noises*");

    // Only required for chatbot-first applications.
    // The main thread can, after this, be used for something completely different, such as API access.
    // You can also make your own sleep system. This function is just provided as a convenience
    chat.block();
}

