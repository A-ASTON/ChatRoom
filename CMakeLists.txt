cmake_minimum_required(VERSION 3.0.0)
project(chatroom VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(server src/ServerMain.cpp lib/Server.cpp)
add_executable(client src/ClientMain.cpp lib/Client.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(Threads)
target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})