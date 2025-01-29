cmake_minimum_required(VERSION 3.10)

# set the project name
project(sf_chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # для clang

include_directories(
    include
)

add_library(TcpLib include/tcp_library.hpp src/tcp_library.cpp)
add_library(DateTimeLib include/datetime.hpp src/datetime.cpp)
add_library(LoggerLib include/logger_interface.hpp include/logger.hpp src/logger.cpp)

# Сервер
add_executable(chat_server 
    src/chat_server.cpp 
    src/message.cpp 
    src/user.cpp
)

target_link_libraries(chat_server PUBLIC TcpLib DateTimeLib LoggerLib -lpqxx -lpq)

target_include_directories(chat_server PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/TcpLib"
                          "${PROJECT_SOURCE_DIR}/LoggerLib"
                          "${PROJECT_SOURCE_DIR}/DateTimeLib"
                          )

# Клиент
add_executable(chat_client 
    src/chat_client.cpp
    src/message.cpp
)

target_link_libraries(chat_client PUBLIC TcpLib DateTimeLib)

target_include_directories(chat_client PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/TcpLib"
                          "${PROJECT_SOURCE_DIR}/DateTimeLib"
                          )