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

add_library(rpc src/rpc_client_funcs.cpp src/rpc_server_funcs.cpp )
# Add the threading library to the rpc library
find_package (Threads)
TARGET_LINK_LIBRARIES(rpc ${CMAKE_THREAD_LIBS_INIT})

add_executable(binder src/binder.cpp)
add_executable(server src/server.c src/server_function_skels.c src/server_functions.c)

TARGET_LINK_LIBRARIES(server rpc)
