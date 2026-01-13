cmake_minimum_required(VERSION 3.10)
project(jot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags for performance
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -funroll-loops")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG") # Disable assertions in release

# Size optimization for embedded systems
if(RASPBERRY_PI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os") # Optimize for size
endif()

# Find ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Include headers from assets directory
include_directories(${CMAKE_SOURCE_DIR}/assets)

# Add executable with all source files
add_executable(jot
    assets/main.cpp
    assets/main_loop.cpp
    assets/input_handler.cpp
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

