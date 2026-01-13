#include <ncurses.h>
#include "config.h"
#include "file_operations.h"
#include "cloud_sync.h"
#include "main_loop.h"

// Main entry point
int main() {
    // Ensure files and templates directories exist
    ensureDirectories();

    // Load cloud sync settings from config file
    loadSettings();

    // Sync from cloud on startup (download any changes)
    if (g_cloudSyncEnabled) {
        syncFromCloud();
    }

    // Initialize ncurses
    initscr();
    cbreak(); // Disable line buffering
    noecho(); // Don't echo input
    keypad(stdscr, TRUE); // Enable arrow keys
    curs_set(0); // Hide cursor

    // Run the main application loop
    runMainLoop();

    // Clean up ncurses
    endwin();

    return 0;
}
