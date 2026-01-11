cmake_minimum_required(VERSION 3.10)
project(typewriter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Add executable
add_executable(typewriter main.cpp)

# Link ncurses
target_link_libraries(typewriter ${CURSES_LIBRARIES})

