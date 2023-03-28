#pragma once

#include "Command.hpp"

namespace stackchat {

class MultiLevelCommand : public Command {
private:
    std::map<std::string, std::shared_ptr<Command>> nextLevel;

    std::shared_ptr<Command> currLevel;

    std::string stringifiedSubcommandList;
public:
    /**
     * The nextLevel parameter is used to define what comes after this level.
     *
     * Assume the command name is "a", and "b", "c", and "d" are subcommands; that would be supplied for nextLevel.
     * currLevel, on the other hand, is what's run if _only_ "a" is supplied. If null, an error message is returned instead.
     */
    MultiLevelCommand(const decltype(nextLevel)& nextLevel,
                      std::shared_ptr<Command> currLevel = nullptr);

    virtual void onMessage(
        Room& room,
        const ChatEvent& receivedMessage,
        const std::vector<std::string>& vArgs
    ) override;


};

}
