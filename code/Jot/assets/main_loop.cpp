#include "main_loop.h"
#include "config.h"
#include "text.h"
#include "file_operations.h"
#include "ui_display.h"
#include "input_handler.h"
#include <ncurses.h>
#include <vector>
#include <string>

void runMainLoop() {
    std::vector<std::string> files = getFiles();
    int selectedIndex = 0;
    int currentPage = 0;
    std::string statusMessage = TEXT_MSG_IDLE;
    bool running = true;
    bool needsRefresh = true; // Start with true to populate initial file list

    while (running) {
        // Only refresh file list when needed (after operations that modify filesystem)
        if (needsRefresh) {
            files = getFiles();
            needsRefresh = false;
        }

        // Calculate pagination values (cached in loop for performance)
        const int maxFilesPerColumn = LINES - TOP_OFFSET - BOTTOM_OFFSET;
        const int filesPerPage = maxFilesPerColumn * MAX_COLUMNS;
        const int totalPages = files.empty() ? 1 : (files.size() + filesPerPage - 1) / filesPerPage;

        // Ensure currentPage is valid
        if (currentPage >= totalPages) {
            currentPage = totalPages - 1;
        }
        if (currentPage < 0) {
            currentPage = 0;
        }

        // Ensure selectedIndex is valid
        if (!files.empty() && selectedIndex >= static_cast<int>(files.size())) {
            selectedIndex = files.size() - 1;
        }
        if (selectedIndex < 0) {
            selectedIndex = 0;
        }

        // Update currentPage based on selectedIndex
        if (!files.empty()) {
            currentPage = selectedIndex / filesPerPage;
        }

        // Draw the UI
        drawUI(files, selectedIndex, currentPage, statusMessage);

        // Get user input
        int ch = getch();

        // Handle the input
        handleInput(ch, files, selectedIndex, currentPage, statusMessage, running,
                   maxFilesPerColumn, filesPerPage, totalPages, needsRefresh);
    }
}

