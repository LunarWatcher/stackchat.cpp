add_library(stackchat
    stackchat/StackChat.cpp
)

target_include_directories(stackchat PUBLIC ./)
target_include_directories(stackchat PUBLIC ${CONAN_INCLUDE_DIRS})

target_link_libraries(stackchat ${CONAN_LIBS})

# vim:ft=cmake
