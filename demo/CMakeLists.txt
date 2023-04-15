function (demo_executable name file)
    add_executable(${name} src/${file})
    target_link_libraries(${name} stackchat)
endfunction()



demo_executable(sandbox-announcement SandboxAnnouncement.cpp)
demo_executable(command-structures CommandStructures.cpp)
demo_executable(listeners ListenerDemo.cpp)
demo_executable(full-cov FullNetworkCoverage.cpp)
