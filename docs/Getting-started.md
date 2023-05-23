# Getting started


## Requirements

As already spoiled in the main README, you need a C++20 compiler and a fairly recent CMake (3.10+). 

## OS support

Theoretically, all operating systems are supported. However, because Windows is Windows, and I use Linux, there is always a chance it's broken.

If you can't get it to compile, open an issue. The primary goal is to support Linux, MacOS, and Windows. The main problem here is that I'm a Linux user, I don't own a Mac, and compiling on Windows is fucking miserable, so I'd rather not unless I have to.


## Linking to your project

There are obviously many options here, but the two officially recommended options (with no particular preference; they're both equally good), re:

### Option 1: Submodules

Assuming you're using Git, it's fairly straight-forward. You `git clone https://github.com/LunarWatcher/stackchat.cpp`, and add
```cmake
add_submodule(stackchat.cpp)
```

to your CMakeLists.txt.

Linking is trivial:
```cmake
target_link_libraries(your-program stackchat)
```

### Option 2: FetchContent

You can use FetchContent as well:
```cmake
include (FetchContent)
FetchContent_Declare(stackchat
    GIT_REPOSITORY https://github.com/LunarWatcher/stackchat.cpp)
FetchContent_MakeAvailable(stackchat)
```

Note that due to the lack of versioning, you'll have to figure out how you want to deal with the rest on your own. You can pin it to a commit, or you can leave it as-is. Proper tags may eventually be created as the library stabilises, but this is a while out.

Linking is trivial:
```cmake
target_link_libraries(your-program stackchat)
```

### Linux note

Due to SSL being used, Linux users have to install libssl-dev (Debian and derivatives; see your package manager for the appropriate name). Windows and Apple users default to WinSSL and DarwinSSL respectively. This is a consequence of OpenSSL not being CMake-based, and I don't feel like spending time on finding a CMake wrapper for the near foreseeable future.

## After installing

You should now have access to the library. See [this document](Connecting-to-chat.md) to get started with the code, or take a look at the demos.
