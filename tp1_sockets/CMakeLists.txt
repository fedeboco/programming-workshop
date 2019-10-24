cmake_minimum_required(VERSION 3.10)
project(v6 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(v6
        board.c
        board.h
        client.c
        client.h
        main.c
        protocol.c
        protocol.h
        server.c
        server.h
        socket.c
        socket.h
        sudoku.c
        sudoku.h)
