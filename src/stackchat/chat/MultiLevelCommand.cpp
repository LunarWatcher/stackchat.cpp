#include "MultiLevelCommand.hpp"

namespace stackchat {

MultiLevelCommand::MultiLevelCommand(const decltype(nextLevel)& nextLevel,
                                     std::shared_ptr<Command> currLevel) : nextLevel(nextLevel), currLevel(currLevel) {
    for (auto& [k, _] : nextLevel) {
        if (stringifiedSubcommandList.size() != 0) {
            stringifiedSubcommandList += ", ";
        }
        stringifiedSubcommandList += k;
    }
}

void MultiLevelCommand::onMessage(
    Room &room,
    const ChatEvent &receivedMessage,
    const std::vector<std::string> &vArgs) {
 

    if (vArgs.size() == 0) {
        if (!currLevel) {
            room.reply(receivedMessage, "You need to supply a subcommand. Valid subcommands: " + stringifiedSubcommandList);
        } else {
            currLevel->onMessage(room, receivedMessage, vArgs);
        }
    } else {
        std::string command = vArgs[0];
        std::vector<std::string> vArgsNew = vArgs;
        vArgsNew.erase(vArgsNew.begin());

        if (!nextLevel.contains(command)) {
            room.reply(receivedMessage, "You need to supply a subcommand. Valid subcommands: " + stringifiedSubcommandList);
        } else {
            nextLevel[command]->onMessage(room, receivedMessage, vArgsNew);
        }
    }
}

}
