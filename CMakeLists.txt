cmake_minimum_required(VERSION 3.27)
project(sockets C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(server src/server.c
        src/Rule.c
        src/RuleFile.c
        src/QueryList.c
        src/Node.c
        src/RuleList.c
        src/RuleHandler.c)
add_executable(client src/client.c)
