cmake_minimum_required(VERSION 3.10)
project(jot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Include headers from assets directory
include_directories(${CMAKE_SOURCE_DIR}/assets)

# Add executable with all source files
add_executable(jot
    assets/main.cpp
    assets/config.cpp
    assets/text.cpp
    assets/lock_system.cpp
    assets/file_operations.cpp
    assets/cloud_sync.cpp
    assets/ui_display.cpp
    assets/settings_menu.cpp
)

# Set output directory to project root so ./jot is created there
set_target_properties(jot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Link ncurses
target_link_libraries(jot ${CURSES_LIBRARIES})

