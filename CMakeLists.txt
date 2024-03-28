cmake_minimum_required(VERSION 3.10)
project(stackchat.cpp)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message (STATUS "stackchat.cpp: Running standalone")
    set (ENABLE_DEMOS ON)
else()
    message (STATUS "stackchat.cpp: Running as a submodule; disabling demos.")
    set (ENABLE_DEMOS OFF)
endif()

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SPDLOG_FMT_EXTERNAL ON CACHE STRING "" FORCE)
set(CURL_ZLIB "AUTO" CACHE STRING "" FORCE)
set(USE_TLS ON CACHE STRING "" FORCE)

include(FetchContent)
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9.1.0)
FetchContent_MakeAvailable(fmt)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.1)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(nlohmann GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.2)
FetchContent_MakeAvailable(nlohmann)

FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.11.0)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(ixwebsocket GIT_REPOSITORY https://github.com/machinezone/IXWebSocket
    GIT_TAG dc8807ec9d25f602d94f023e40a896de820d2bee)
FetchContent_MakeAvailable(ixwebsocket)

FetchContent_Declare(stc GIT_REPOSITORY https://github.com/LunarWatcher/stc)
FetchContent_MakeAvailable(stc)

add_subdirectory(src)
if (ENABLE_DEMOS)
    add_subdirectory(demo)
endif()
#add_subdirectory(tests)

#add_custom_target(test
    #COMMAND tests
    #DEPENDS tests
    #COMMENT "Test stackchat.cpp")
# vim:ft=cmake
