cmake_minimum_required(VERSION 2.8.7)

#Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

#Project Name
project(cs454a3)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

#Set the Include dir
include_directories(include)

#Get threading
find_package (Threads)

# Libraries
# -----------------------------
    add_library(
        rpc

        src/client/rpc_client_funcs.cpp
        src/client/client_sender.cpp

        src/server/rpc_server_funcs.cpp
        src/server/server.cpp
        src/server/server_receiver.cpp
        src/server/server_sender.cpp

        src/common/common_defs.cpp
        src/common/message.cpp
        src/common/receiver.cpp
        src/common/sender.cpp
    )

    TARGET_LINK_LIBRARIES(rpc ${CMAKE_THREAD_LIBS_INIT})
# -----------------------------


#Binaries
# -----------------------------
    add_executable(binder src/binder/binder.cpp)
    TARGET_LINK_LIBRARIES(binder rpc)

    add_executable(
        server
        src/test/server/server.c
        src/test/server/server_function_skels.c
        src/test/server/server_functions.c
    )
    TARGET_LINK_LIBRARIES(server rpc)

    add_executable(
        long_argtypes_server
        src/test/server/long_argtypes_server.c
    )
    TARGET_LINK_LIBRARIES(long_argtypes_server rpc)

    add_executable(
        client        
        src/test/client/client.c
    )
    TARGET_LINK_LIBRARIES(client rpc)

# -----------------------------
