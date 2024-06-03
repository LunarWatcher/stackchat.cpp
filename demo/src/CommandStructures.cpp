#include "stackchat/StackChat.hpp"
#include "stackchat/chat/ChatEvent.hpp"
#include "stackchat/chat/Command.hpp"
#include "stackchat/chat/MultiLevelCommand.hpp"

#include <iostream>
#include <string>

class InterjectChatCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        r.reply(ev, "I'd like to interject for a moment. What you're referring to as chat login is, in fact, an eternal source of pain and suffering.");
    }
};
class GNUCopypastaCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        r.reply(ev, R"(I'd just like to interject for a moment. What you're referring to as Linux, is in fact, GNU/Linux, or as I've recently taken to calling it, GNU plus Linux. Linux is not an operating system unto itself, but rather another free component of a fully functioning GNU system made useful by the GNU corelibs, shell utilities and vital system components comprising a full OS as defined by POSIX.)");
    }
};
class CursedCopypastaCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        r.reply(ev, R"(I'd just wike to intewject fow a moment. What you'we wefewwing to as linux, is in fact, gnu/linux, ow as i've wecentwy taken to cawwing it, gnu pwus linux. linux is not an opewating system unto itsewf, but wathew anothew fwee component of a fuwwy functioning gnu system made usefuw by the gnu cowewibs, sheww utiwities and vitaw system components compwising a fuww os as defined by posix.)");
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

    // A MultiLevelCommand is just a utility that wraps other commands. It can contain a command for the current level (see its documentation),
    // but this is not required.
    chat.registerCommand("interject", std::make_shared<stackchat::MultiLevelCommand>(
            // The explicit specifier is required due to type inferrence
            std::map<std::string, std::shared_ptr<stackchat::Command>>{
                {"chat", std::make_shared<InterjectChatCommand>()},
                {"unix",
                    // And yes, multi-level commands nest as far as chat allows.
                    std::make_shared<stackchat::MultiLevelCommand>(
                        std::map<std::string, std::shared_ptr<stackchat::Command>> {
                            {"uwu", std::make_shared<CursedCopypastaCommand>()}
                        },
                        // This is a demonstration of an on-level command; running ~interject unix will print this by default, while the uwu alternative
                        // can be separately executed.
                        //
                        // This example does not include a help command, largely because it hasn't been implemented at the time of writing, and because
                        // it makes more sense to demonstrate help commands separately.
                        std::make_shared<GNUCopypastaCommand>()
                    )
                }
            }
    ));

    chat.join(stackchat::StackSite::STACKOVERFLOW, 1);
    chat.sendTo(stackchat::StackSite::STACKOVERFLOW, 1, "Greetings, humans");

    chat.block();
}

