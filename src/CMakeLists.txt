add_library(stackchat
    stackchat/StackChat.cpp

    stackchat/rooms/Room.cpp

    stackchat/web/Browser.cpp
)

target_include_directories(stackchat PUBLIC ./)

target_link_libraries(stackchat PUBLIC cpr::cpr nlohmann_json::nlohmann_json
    spdlog::spdlog ixwebsocket::ixwebsocket)

# vim:ft=cmake
