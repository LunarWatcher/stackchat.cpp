# stackchat.cpp


C++ library for interacting with chat on [Stack Exchange](//stackexchange.com).

## Archive notice

Due to SE configuring cloudflare increasingly aggressively, the login procedure used by this library no longer works reliably. Find other libraries, or even better, find another chat platform. It may still work, but due to [SE's ongoing effort to fuck over the community as hard as possible](https://github.com/LunarWatcher/se-data-dump-transformer/?tab=readme-ov-file#2024-data-dump-restriction-attempt) at every opportunity they get, I have left the platform. The cloudflare bug that fucked over boson [pushed it over on discord](https://github.com/LunarWatcher/boson-light.cpp/blob/ab51646c892f68bd09dea0de6ecfb6c35cd3e15a/src/boson/ChatProvider.cpp#L49-L131), where it has been operating without issues. Since the API is threatened too (at the time of writing, v3 of the API is being planned with major breaking data  removals that will break many scripts and bots), there's no point in trying to keep this library alive.

Switch to a better chat platform, and switch to a better Q&A platform that doesn't actively hate its community.

## Requirements

* A C++20 compiler

Libraries are downloaded automagically by FetchContent.
