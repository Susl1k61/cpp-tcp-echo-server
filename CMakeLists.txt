cmake_minimum_required(VERSION 3.20)
project(NetworkHomework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Task2_Server Task2_Synchronous/Server.cpp)
add_executable(Task2_Client Task2_Synchronous/Client.cpp)
add_executable(Task3_Server Task3_Asynchronous/ServerAsync.cpp)
add_executable(Task3_Client Task3_Asynchronous/Client.cpp)

if(WIN32)
    target_link_libraries(Task2_Server PRIVATE ws2_32)
    target_link_libraries(Task2_Client PRIVATE ws2_32)
    target_link_libraries(Task3_Server PRIVATE ws2_32)
    target_link_libraries(Task3_Client PRIVATE ws2_32)
endif()