#include "stackchat/StackChat.hpp"
#include "stackchat/chat/ChatEvent.hpp"
#include "stackchat/chat/Command.hpp"
#include "stackchat/rooms/Room.hpp"

#include <iostream>
#include <string>

class CodeCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        r.sendMessage(R"(#include <iostream>

int main() {
    std::cout << "Hewwo, world" << std::endl;
}
)", stackchat::MessageType::CODE);
        }
};

class CopypastaCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        auto ids = r.reply(ev, R"(It's a well-known fact that Americans have a bit of a struggle when it comes to using the metric system. Despite the rest of the world using this logical, easy-to-understand system of measurement, Americans still insist on using their outdated imperial units.

But you know what? I'm not here to judge. In fact, I think it's kind of endearing how Americans come up with their own unique units of measurement. Here are just a few examples:

  *  Burgers per football field
  *  Bald eagles per apple pie
  *  Liberty per bald eagle
  *  Guns per capita, squared
  *  Trumps per minute of cable news
  *  Miles per gallon of freedom
  *  Star-spangled banner wavelengths
  *  'Murica per square inch of land
  *  Baseballs per hotdog

So the next time someone gives you grief for not using the metric system, just remind them of all the amazing American units of measurement that exist. After all, who needs centimeters and kilograms when you can have the much more intuitive freedom per assault rifle squared?)");
        std::string idstr = "";
        for (auto& id : ids) {
            idstr += (idstr.size() == 0 ? "" : ", ") + std::to_string(id);
        }
        r.sendMessage("Messages sent: " + idstr);
    }
};

class QuoteCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        auto ids = r.reply(
            ev,
            "The whole course of human history may depend on a change of heart in one solitary and even humble individual - for it is in the solitary mind and soul of the individual that the battle between good and evil is waged and ultimately won or lost.", 
            stackchat::MessageType::QUOTE);
        std::string idstr = "";
        for (auto& id : ids) {
            idstr += (idstr.size() == 0 ? "" : ", ") + std::to_string(id);
        }
        r.sendMessage("Messages sent: " + idstr);
    }
};

class LoremCommand : public stackchat::Command {
public:
    void onMessage(stackchat::Room& r, const stackchat::ChatEvent& ev, const std::vector<std::string>&) override {
        auto ids = r.reply(ev, 
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In aliquet diam et sapien tincidunt, eget ultrices elit facilisis. Praesent eu felis lorem. Nam in nibh eu massa iaculis fermentum sit amet eget diam. Quisque id dictum nulla, vitae fringilla eros. Nullam ligula mi, vestibulum vehicula varius non, convallis et nunc. Fusce in dapibus arcu. Vivamus at convallis odio, et fringilla ex. Sed non malesuada purus, sed auctor magna.  Sed sagittis ultrices eros sed sodales. Fusce rutrum faucibus ex, id convallis nunc posuere a. Duis et commodo augue. Duis quis mi dictum, lobortis nisi quis, interdum lorem. Nullam elit dui, cursus eget eros non, rutrum vulputate metus. Suspendisse in eros rutrum nunc tempus facilisis. Donec in pellentesque erat, ut congue sapien.  Nam facilisis tellus nec urna consectetur pharetra. Aenean eget tellus ac massa tincidunt ullamcorper. Sed lobortis fringilla dolor. Nullam ullamcorper rutrum varius. Nulla blandit eu quam non tincidunt. Aliquam elementum mollis risus cursus egestas. Curabitur pulvinar pharetra molestie. Praesent ex mi, accumsan in vehicula et, feugiat eu elit. In dignissim rhoncus est, in cursus augue sagittis vitae. Praesent at magna mauris. Donec semper urna at sem eleifend, nec dapibus erat elementum. Nunc eu aliquet orci. Morbi vel porttitor mi. Curabitur a venenatis arcu. Suspendisse tempor consequat fringilla.");

        std::string idstr = "";
        for (auto& id : ids) {
            idstr += (idstr.size() == 0 ? "" : ", ") + std::to_string(id);
        }
        r.sendMessage("Messages sent: " + idstr);
        ids = r.reply(ev, 
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In aliquet diam et sapien tincidunt, eget ultrices elit facilisis. Praesent eu felis lorem. Nam in nibh eu massa iaculis fermentum sit amet eget diam. Quisque id dictum nulla, vitae fringilla eros. Nullam ligula mi, vestibulum vehicula varius non, convallis et nunc. Fusce in dapibus arcu. Vivamus at convallis odio, et fringilla ex. Sed non malesuada purus, sed auctor magna.  Sed sagittis ultrices eros sed sodales. Fusce rutrum faucibus ex, id convallis nunc posuere a. Duis et commodo augue. Duis quis mi dictum, lobortis nisi quis, interdum lorem. Nullam elit dui, cursus eget eros non, rutrum vulputate metus. Suspendisse in eros rutrum nunc tempus facilisis. Donec in pellentesque erat, ut congue sapien.  Nam facilisis tellus nec urna consectetur pharetra. Aenean eget tellus ac massa tincidunt ullamcorper. Sed lobortis fringilla dolor. Nullam ullamcorper rutrum varius. Nulla blandit eu quam non tincidunt. Aliquam elementum mollis risus cursus egestas. Curabitur pulvinar pharetra molestie. Praesent ex mi, accumsan in vehicula et, feugiat eu elit. In dignissim rhoncus est, in cursus augue sagittis vitae. Praesent at magna mauris. Donec semper urna at sem eleifend, nec dapibus erat elementum. Nunc eu aliquet orci. Morbi vel porttitor mi. Curabitur a venenatis arcu. Suspendisse tempor consequat fringilla.",
                stackchat::MessageType::NORMAL,
                stackchat::MessageLengthPolicy::BREAK);
        idstr = "";
        for (auto& id : ids) {
            idstr += (idstr.size() == 0 ? "" : ", ") + std::to_string(id);
        }
        r.sendMessage("Messages sent: " + idstr);
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

    chat.registerCommand("code", std::make_shared<CodeCommand>());
    chat.registerCommand("muricah", std::make_shared<CopypastaCommand>());
    chat.registerCommand("quote", std::make_shared<QuoteCommand>());
    chat.registerCommand("lorem", std::make_shared<LoremCommand>());

    // There are many ways to deal with chat joining, as well as announcements and config, but this is left as an exercise to the reader, because there
    // are very, very many ways to implement it, depending on what the bot is used for.
    chat.join(stackchat::StackSite::META_STACKEXCHANGE, 1696);
    // As an aside, this isn't really the only way to do join announcements.
    // It would be possible to hook up a join listener, and check if the bot triggered the event.

    // Only required for chatbot-first applications.
    // The main thread can, after this, be used for something completely different, such as API access.
    // You can also make your own sleep system. This function is just provided as a convenience
    chat.block();
}

