cmake_minimum_required(VERSION 2.8.7)

#Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

#Project Name
project(cs454a3)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++0x support. Please use a different C++ compiler.")
endif()

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


# Replace the default add_executable function with one that automatically
# links in the rpc library
macro (add_executable _name)
    # invoke built-in add_executable
    _add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} rpc)
    endif()
endmacro()


#Binaries
# -----------------------------
    add_executable(binder src/binder/binder.cpp)

    add_executable(
        server
        test/server/server.c
        test/server/server_function_skels.c
        test/server/server_functions.c
    )

    add_executable(
        server2
        test/server/server2.c
        test/server/server_function_skels.c
        test/server/server_functions.c
    )

    add_executable(
        long_argtypes_server
        test/server/long_argtypes_server.c
    )

    add_executable(
        server_error
        test/server/server_error.c
        test/server/server_function_skels.c
        test/server/server_functions.c
    )

    add_executable(
        client        
        test/client/client.c
    )

    add_executable(
        client_round_robin
        test/client/client_round_robin.c
    )

    add_executable(
        client_terminator
        test/client/client_terminator.c
    )

    add_executable(
        client_long
        test/client/client_long.c
    )

    add_executable(
        client_error
        test/client/client_error.c
    )

# -----------------------------
