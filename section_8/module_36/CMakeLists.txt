cmake_minimum_required(VERSION 3.12)

project(ChatServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()


set(CHAT_LIBRARIES)
set(Qt5Modules Core Widgets)
foreach(QtModule IN LISTS Qt5Modules)
    list(APPEND CHAT_LIBRARIES Qt5::${QtModule})
endforeach(QtModule)

find_package(Qt5 COMPONENTS ${Qt5Modules} REQUIRED)

file(GLOB_RECURSE SRC_FILES sources/*.cpp)
file(GLOB_RECURSE HEADERS_FILES sources/*.h)

file(GLOB_RECURSE RC_FILES sources/*.qrc)
file(GLOB_RECURSE UI_FILES sources/*.ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt5_wrap_ui(QUI ${UI_FILES})
qt5_add_resources(QRC ${RC_FILES})

# include_directories(
# ${QT_INCLUDES}
# )

add_executable(ChatServer ${SRC_FILES} ${HEADERS_FILES} ${RC_FILES} ${UI_FILES}
    sources/mainwindow.h sources/mainwindow.cpp sources/mainwindow.ui
    sources/custom_event.h)

target_link_libraries(ChatServer
    PUBLIC
    ${CHAT_LIBRARIES}
     -lpqxx -lpq
)
