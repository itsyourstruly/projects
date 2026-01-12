#include "ui_display.h"
#include "config.h"
#include "text.h"
#include "file_operations.h"
#include "lock_system.h"
#include <ncurses.h>
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

// Global selection state (definitions)
std::set<std::string> g_selectedPaths;
bool g_selectMode = false;
int g_anchorIndex = -1;
std::string g_anchorPath = "";

// Undo action
UndoAction g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};

// Selection helper functions
bool isPathSelected(const std::string& fullPath) {
    return g_selectedPaths.find(fullPath) != g_selectedPaths.end();
}

void toggleSelection(const std::string& fullPath) {
    if (isPathSelected(fullPath)) {
        g_selectedPaths.erase(fullPath);
    } else {
        g_selectedPaths.insert(fullPath);
    }
}

// Undo functionality
bool performUndo(std::string& statusMessage) {
    if (g_lastAction.type == UNDO_NONE) {
        statusMessage = TEXT_MSG_NOTHING_TO_UNDO;
        return false;
    }

    try {
        switch (g_lastAction.type) {
            case UNDO_CREATE_FILE:
            case UNDO_CREATE_FOLDER:
                if (fs::exists(g_lastAction.path1)) {
                    fs::remove_all(g_lastAction.path1);
                    statusMessage = std::string(TEXT_MSG_UNDONE) + "created " + fs::path(g_lastAction.path1).filename().string();
                    g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                    return true;
                }
                break;

            case UNDO_DELETE:
                if (g_lastAction.wasDirectory) {
                    fs::create_directory(g_lastAction.path1);
                } else {
                    std::ofstream outFile(g_lastAction.path1);
                    if (outFile.is_open()) {
                        outFile << g_lastAction.content;
                        outFile.close();
                    }
                }
                statusMessage = std::string(TEXT_MSG_UNDONE) + "deleted " + fs::path(g_lastAction.path1).filename().string();
                g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                return true;

            case UNDO_RENAME:
                if (fs::exists(g_lastAction.path2)) {
                    fs::rename(g_lastAction.path2, g_lastAction.path1);
                    statusMessage = std::string(TEXT_MSG_UNDONE) + "renamed " + fs::path(g_lastAction.path2).filename().string();
                    g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                    return true;
                }
                break;

            case UNDO_MOVE: {
                int restoredCount = 0;
                for (size_t i = 0; i < g_lastAction.movedPaths.size() && i < g_lastAction.destPaths.size(); i++) {
                    if (fs::exists(g_lastAction.destPaths[i])) {
                        fs::rename(g_lastAction.destPaths[i], g_lastAction.movedPaths[i]);
                        restoredCount++;
                    }
                }
                if (restoredCount > 0) {
                    statusMessage = std::string(TEXT_MSG_UNDONE) + "moved " + std::to_string(restoredCount) + " item(s)";
                    g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                    return true;
                }
                break;
            }

            default:
                break;
        }
    } catch (...) {
        statusMessage = TEXT_MSG_UNDO_FAILED;
        return false;
    }

    statusMessage = TEXT_MSG_UNDO_FAILED;
    return false;
}

void drawSeparator(int y) {
    int width = COLS;
    for (int x = 0; x < width; x++) {
        mvprintw(y, x, "-");
    }
}

void drawUI(const std::vector<std::string>& files, int selectedIndex, int currentPage, const std::string& message) {
    clear();

    // Header
    mvprintw(0, 0, TEXT_HEADER);

    // Status in upper right
    std::string statusText = std::string(TEXT_STATUS_PREFIX) + message;
    mvprintw(0, COLS - statusText.length(), "%s", statusText.c_str());

    // Show current path
    if (currentPath == BASE_DIR) {
        mvprintw(1, 0, TEXT_HOME_FOLDER);
    } else {
        std::string displayPath = currentPath;
        if (displayPath.substr(0, BASE_DIR.length()) == BASE_DIR) {
            displayPath = displayPath.substr(BASE_DIR.length() + 1);
        }
        mvprintw(1, 0, TEXT_YOU_ARE_IN, displayPath.c_str());
    }

    // Show last modified time for selected item (upper right, below status)
    if (!files.empty() && selectedIndex >= 0 && selectedIndex < files.size()) {
        std::string fullPath = getFullPath(files[selectedIndex]);
        std::string timeAgo = getTimeAgo(fullPath);
        if (!timeAgo.empty()) {
            std::string timeText = "Modified: " + timeAgo;
            mvprintw(1, COLS - timeText.length(), "%s", timeText.c_str());
        }
    }

    drawSeparator(2);

    // Calculate pagination
    int maxFilesPerColumn = LINES - TOP_OFFSET - BOTTOM_OFFSET;
    int filesPerPage = maxFilesPerColumn * MAX_COLUMNS;
    int totalPages = files.empty() ? 1 : (files.size() + filesPerPage - 1) / filesPerPage;
    int startIndex = currentPage * filesPerPage;
    int endIndex = std::min(startIndex + filesPerPage, (int)files.size());

    if (files.empty()) {
        mvprintw(TOP_OFFSET, 2, TEXT_NO_FILES);
    } else {
        // Display files in columns
        for (int i = startIndex; i < endIndex; i++) {
            int column = (i - startIndex) / maxFilesPerColumn;
            int row = (i - startIndex) % maxFilesPerColumn;
            int x = column * COLUMN_WIDTH;
            int y = TOP_OFFSET + row;

            std::string displayName = files[i];
            std::string fullPath = getFullPath(files[i]);
            bool selected = isPathSelected(fullPath);
            bool isLocked = isFileLockedByDisplay(files[i]);

            // Apply selection highlighting
            if (i == selectedIndex) {
                attron(A_REVERSE);
            }

            // Apply bold for locked files
            if (isLocked) {
                attron(A_BOLD);
            }

            // Truncate if too long
            if (displayName.length() > COLUMN_WIDTH - 2) {
                displayName = displayName.substr(0, COLUMN_WIDTH - 5) + "...";
            }

            // Show selection marker
            if (selected) {
                mvprintw(y, x, "> %s", displayName.c_str());
            } else {
                mvprintw(y, x, "  %s", displayName.c_str());
            }

            if (isLocked) {
                attroff(A_BOLD);
            }

            if (i == selectedIndex) {
                attroff(A_REVERSE);
            }
        }
    }

    // Footer
    int footerY = LINES - 6;
    drawSeparator(footerY);

    // Page indicator
    if (totalPages > 1) {
        mvprintw(footerY + 1, 0, TEXT_PAGE_INDICATOR, currentPage + 1, totalPages);
    }

    // Controls - single line that wraps automatically
    mvprintw(footerY + 2, 0, TEXT_CONTROLS);

    // Build one continuous control string
    std::string allControls = std::string("  ") + TEXT_ARROWS + " | " + TEXT_ENTER +
        "  n:New  o:Folder  r:Rename  d:Delete  c:Convert  l:Lock  s:Select  v:Move  f:Find  z:Undo  b:Back  t:Settings  q:Quit";

    // Print the controls starting at footerY + 3, let it wrap naturally
    int startY = footerY + 3;
    int currentX = 0;
    int currentY = startY;

    for (size_t i = 0; i < allControls.length(); i++) {
        if (currentX >= COLS) {
            currentX = 0;
            currentY++;
        }
        mvaddch(currentY, currentX, allControls[i]);
        currentX++;
    }

    // Clear remaining lines in footer area
    for (int y = currentY + 1; y < LINES; y++) {
        move(y, 0);
        clrtoeol();
    }

    refresh();
}

std::string getUserInput(const std::string& prompt) {
    int y = LINES - 5;

    mvprintw(y, 0, "%s", prompt.c_str());
    clrtoeol();
    refresh();

    curs_set(1);
    noecho();

    std::string input;
    int ch;

    while (true) {
        ch = getch();

        if (ch == 27) { // ESC
            curs_set(0);
            return "";
        } else if (ch == '\n' || ch == KEY_ENTER || ch == 10) {
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8 || ch == 263) {
            if (!input.empty()) {
                input.pop_back();
                mvprintw(y, 0, "%s", prompt.c_str());
                clrtoeol();
                if (!input.empty()) {
                    printw("%s", input.c_str());
                }
                refresh();
            }
        } else if (ch >= 32 && ch <= 126 && ch != 127) {
            input += (char)ch;
            addch(ch);
            refresh();
        }
    }

    curs_set(0);
    return input;
}

int selectTemplate(const std::vector<std::string>& templates) {
    int selectedTemplate = 0;
    bool selecting = true;

    while (selecting) {
        clear();
        mvprintw(1, 0, TEXT_PROMPT_SELECT_TEMPLATE);
        drawSeparator(2);

        for (size_t i = 0; i < templates.size(); i++) {
            if (i == selectedTemplate) {
                attron(A_REVERSE);
            }
            mvprintw(3 + i, 2, "%s", templates[i].c_str());
            if (i == selectedTemplate) {
                attroff(A_REVERSE);
            }
        }

        drawSeparator(3 + templates.size() + 1);
        mvprintw(3 + templates.size() + 2, 0, TEXT_PROMPT_TEMPLATE_NAV);

        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP:
                if (selectedTemplate > 0) selectedTemplate--;
                break;
            case KEY_DOWN:
                if (selectedTemplate < (int)templates.size() - 1) selectedTemplate++;
                break;
            case '\n':
            case KEY_ENTER:
                return selectedTemplate;
            case 27: // ESC
                return -1;
        }
    }

    return -1;
}

// Search mode functionality
void enterSearchMode(const std::vector<std::string>& allFiles, int& selectedIndex, std::string& statusMessage) {
    std::string searchQuery;
    std::vector<std::string> matchedFiles;
    bool searching = true;
    int selectedResultIndex = 0;

    // Get all files recursively from current directory and subdirectories
    std::vector<std::string> allFilesRecursive = getAllFilesRecursive(currentPath);

    curs_set(1); // Show cursor for typing

    while (searching) {
        // Filter files based on search query
        matchedFiles.clear();
        if (!searchQuery.empty()) {
            for (const auto& file : allFilesRecursive) {
                std::string lowerFile = file;
                std::string lowerQuery = searchQuery;

                // Convert to lowercase for case-insensitive search
                std::transform(lowerFile.begin(), lowerFile.end(), lowerFile.begin(), ::tolower);
                std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

                if (lowerFile.find(lowerQuery) != std::string::npos) {
                    matchedFiles.push_back(file);
                }
            }

            // Keep selectedResultIndex in bounds
            if (selectedResultIndex >= (int)matchedFiles.size()) {
                selectedResultIndex = matchedFiles.size() - 1;
            }
            if (selectedResultIndex < 0 && !matchedFiles.empty()) {
                selectedResultIndex = 0;
            }
        } else {
            selectedResultIndex = 0;
        }

        // Draw the normal UI but with search bar instead of separator
        clear();

        // Header
        mvprintw(0, 0, TEXT_HEADER);

        // Status in upper right
        std::string statusText = std::string(TEXT_STATUS_PREFIX) + statusMessage;
        mvprintw(0, COLS - statusText.length(), "%s", statusText.c_str());

        // Show current path
        if (currentPath == BASE_DIR) {
            mvprintw(1, 0, TEXT_HOME_FOLDER);
        } else {
            std::string displayPath = currentPath;
            if (displayPath.substr(0, BASE_DIR.length()) == BASE_DIR) {
                displayPath = displayPath.substr(BASE_DIR.length() + 1);
            }
            mvprintw(1, 0, TEXT_YOU_ARE_IN, displayPath.c_str());
        }

        drawSeparator(2);

        // Display files (either all files or filtered)
        const std::vector<std::string>& displayFiles = searchQuery.empty() ? allFiles : matchedFiles;

        int maxFilesPerColumn = LINES - TOP_OFFSET - BOTTOM_OFFSET - 3;
        int filesPerPage = maxFilesPerColumn * MAX_COLUMNS;
        int currentPage = selectedIndex / filesPerPage;
        int startIndex = currentPage * filesPerPage;
        int endIndex = std::min(startIndex + filesPerPage, (int)displayFiles.size());

        if (displayFiles.empty()) {
            if (searchQuery.empty()) {
                mvprintw(TOP_OFFSET, 2, TEXT_NO_FILES);
            } else {
                mvprintw(TOP_OFFSET, 2, TEXT_SEARCH_NO_RESULTS);
            }
        } else {
            for (int i = startIndex; i < endIndex; i++) {
                int column = (i - startIndex) / maxFilesPerColumn;
                int row = (i - startIndex) % maxFilesPerColumn;
                int x = column * COLUMN_WIDTH;
                int y = TOP_OFFSET + row;

                std::string displayName = displayFiles[i];

                if (i == selectedIndex) {
                    attron(A_REVERSE);
                }

                if (displayName.length() > COLUMN_WIDTH - 2) {
                    displayName = displayName.substr(0, COLUMN_WIDTH - 5) + "...";
                }

                mvprintw(y, x, "  %s", displayName.c_str());

                if (i == selectedIndex) {
                    attroff(A_REVERSE);
                }
            }
        }

        // Footer - REPLACE separator with search prompt
        int footerY = LINES - 6;

        // Show best search result (highlighted) before the search prompt
        if (!matchedFiles.empty() && !searchQuery.empty()) {
            std::string bestResult = matchedFiles[selectedResultIndex];

            // Truncate if too long to fit before "Find: "
            int maxResultWidth = COLS - 50; // Leave room for Find: prompt and controls
            if ((int)bestResult.length() > maxResultWidth) {
                bestResult = bestResult.substr(0, maxResultWidth - 3) + "...";
            }

            // Display with highlight
            attron(A_REVERSE);
            mvprintw(footerY, 0, " %s ", bestResult.c_str());
            attroff(A_REVERSE);

            // Move search prompt to after the result
            int promptX = bestResult.length() + 3;
            std::string searchPrompt = std::string("  ") + TEXT_SEARCH_PROMPT + searchQuery;
            mvprintw(footerY, promptX, "%s", searchPrompt.c_str());

            // Show results count on the right side
            std::string resultsText = "(" + std::to_string(matchedFiles.size()) + TEXT_SEARCH_RESULTS;
            mvprintw(footerY, COLS - resultsText.length(), "%s", resultsText.c_str());

            // Position cursor at end of search query
            move(footerY, promptX + searchPrompt.length());
        } else {
            // No results or empty query - just show search prompt
            std::string searchPrompt = std::string(TEXT_SEARCH_PROMPT) + searchQuery;
            mvprintw(footerY, 0, "%s", searchPrompt.c_str());

            // Show results count on the right side if searching
            if (!searchQuery.empty()) {
                std::string resultsText = "(0" + std::string(TEXT_SEARCH_RESULTS);
                mvprintw(footerY, COLS - resultsText.length(), "%s", resultsText.c_str());
            }

            // Position cursor at end of search query
            move(footerY, searchPrompt.length());
        }

        // Clear the rest of the line
        clrtoeol();

        // Page indicator
        if (!displayFiles.empty()) {
            int totalPages = (displayFiles.size() + filesPerPage - 1) / filesPerPage;
            if (totalPages > 1) {
                mvprintw(footerY + 1, 0, TEXT_PAGE_INDICATOR, currentPage + 1, totalPages);
            }
        }

        // Controls
        mvprintw(footerY + 2, 0, TEXT_CONTROLS);
        std::string allControls = std::string("  ") + TEXT_ARROWS + " | " + TEXT_ENTER +
            "  n:New  o:Folder  r:Rename  d:Delete  c:Convert  l:Lock  s:Select  v:Move  f:Find  z:Undo  b:Back  t:Settings  q:Quit";

        int startY = footerY + 3;
        int currentX = 0;
        int currentY = startY;

        for (size_t i = 0; i < allControls.length(); i++) {
            if (currentX >= COLS) {
                currentX = 0;
                currentY++;
            }
            mvaddch(currentY, currentX, allControls[i]);
            currentX++;
        }

        // Clear remaining lines
        for (int y = currentY + 1; y < LINES; y++) {
            move(y, 0);
            clrtoeol();
        }

        refresh();

        int ch = getch();

        if (ch == 27) { // ESC - exit search mode
            searching = false;
            curs_set(0);
            statusMessage = TEXT_MSG_IDLE;
        } else if (ch == KEY_UP) {
            // Navigate up through search results
            if (!matchedFiles.empty() && selectedResultIndex > 0) {
                selectedResultIndex--;
            }
        } else if (ch == KEY_DOWN) {
            // Navigate down through search results
            if (!matchedFiles.empty() && selectedResultIndex < (int)matchedFiles.size() - 1) {
                selectedResultIndex++;
            }
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8 || ch == 263) {
            if (!searchQuery.empty()) {
                searchQuery.pop_back();
                selectedResultIndex = 0; // Reset to first result when query changes
            }
        } else if (ch == 'x' || ch == 'X') { // X - also exits
            searching = false;
            curs_set(0);
            statusMessage = TEXT_MSG_IDLE;
        } else if (ch >= 32 && ch <= 126 && ch != 127) {
            searchQuery += (char)ch;
            selectedResultIndex = 0; // Reset to first result when query changes
        }
    }
}

