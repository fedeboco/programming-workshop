cmake_minimum_required(VERSION 3.10)
project(client)
project(server)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(client client_main.cpp common_socket.cpp Socket.h common_protocol.cpp Protocol.h common_config.cpp Config.h common_command.cpp Command.h common_client.cpp Client.h common_server.cpp common_server.h common_error.cpp common_error.h)
add_executable(server server_main.cpp common_socket.cpp Socket.h common_protocol.cpp Protocol.h common_config.cpp Config.h common_command.cpp Command.h common_client.cpp Client.h common_server.cpp common_server.h common_error.cpp common_error.h)