#include <ncurses.h>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
// Include all modular headers
#include "config.h"
#include "keys.h"
#include "text.h"
#include "lock_system.h"
#include "file_operations.h"
#include "cloud_sync.h"
#include "ui_display.h"
#include "settings_menu.h"
namespace fs = std::filesystem;
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

    std::vector<std::string> files = getFiles();
    int selectedIndex = 0;
    int currentPage = 0;
    std::string statusMessage = TEXT_MSG_IDLE;

    bool running = true;
    while (running) {
        // Refresh file list
        files = getFiles();

        // Calculate pagination values
        int maxFilesPerColumn = LINES - TOP_OFFSET - BOTTOM_OFFSET;
        int filesPerPage = maxFilesPerColumn * MAX_COLUMNS;
        int totalPages = files.empty() ? 1 : (files.size() + filesPerPage - 1) / filesPerPage;

        // Ensure currentPage is valid
        if (currentPage >= totalPages) {
            currentPage = totalPages - 1;
        }
        if (currentPage < 0) {
            currentPage = 0;
        }

        // Ensure selectedIndex is valid
        if (!files.empty() && selectedIndex >= files.size()) {
            selectedIndex = files.size() - 1;
        }
        if (selectedIndex < 0) {
            selectedIndex = 0;
        }

        // Update currentPage based on selectedIndex
        if (!files.empty()) {
            currentPage = selectedIndex / filesPerPage;
        }

        drawUI(files, selectedIndex, currentPage, statusMessage);

        int ch = getch();
        // Status will be updated by actions, or stay as is

        switch(ch) {
            case KEY_UP:
                if (selectedIndex > 0) {
                    // Store the item we're leaving BEFORE moving
                    std::string prevPath = !files.empty() ? getFullPath(files[selectedIndex]) : "";

                    selectedIndex--;

                    // If in selection mode, update selections based on direction from anchor
                    if (g_selectMode && !files.empty() && g_anchorIndex >= 0) {
                        std::string fullPath = getFullPath(files[selectedIndex]);

                        if (selectedIndex < g_anchorIndex) {
                            // Moving up from anchor - select this item
                            g_selectedPaths.insert(fullPath);
                        } else if (selectedIndex > g_anchorIndex) {
                            // Moving back up toward anchor from below - deselect the item we just LEFT
                            if (prevPath != g_anchorPath && !prevPath.empty()) {
                                g_selectedPaths.erase(prevPath);
                            }
                        } else if (selectedIndex == g_anchorIndex) {
                            // Landed exactly on anchor - deselect the item we left
                            if (prevPath != g_anchorPath && !prevPath.empty()) {
                                g_selectedPaths.erase(prevPath);
                            }
                        }
                    }
                }
                break;

            case KEY_DOWN:
                if (!files.empty() && selectedIndex < files.size() - 1) {
                    // Store the item we're leaving BEFORE moving
                    std::string prevPath = getFullPath(files[selectedIndex]);

                    selectedIndex++;

                    // If in selection mode, update selections based on direction from anchor
                    if (g_selectMode && !files.empty() && g_anchorIndex >= 0) {
                        std::string fullPath = getFullPath(files[selectedIndex]);

                        if (selectedIndex > g_anchorIndex) {
                            // Moving down from anchor - select this item
                            g_selectedPaths.insert(fullPath);
                        } else if (selectedIndex < g_anchorIndex) {
                            // Moving back down toward anchor from above - deselect the item we just LEFT
                            if (prevPath != g_anchorPath && !prevPath.empty()) {
                                g_selectedPaths.erase(prevPath);
                            }
                        } else if (selectedIndex == g_anchorIndex) {
                            // Landed exactly on anchor - deselect the item we left
                            if (prevPath != g_anchorPath && !prevPath.empty()) {
                                g_selectedPaths.erase(prevPath);
                            }
                        }
                    }
                }
                break;

            case KEY_LEFT: {
                // Calculate current position
                int pageStartIndex = currentPage * filesPerPage;
                int relativeIndex = selectedIndex - pageStartIndex;
                int currentCol = relativeIndex / maxFilesPerColumn;
                int currentRow = relativeIndex % maxFilesPerColumn;

                if (currentCol > 0) {
                    // Move to previous column (same row)
                    selectedIndex -= maxFilesPerColumn;
                } else if (currentPage > 0) {
                    // Move to previous page, rightmost column
                    currentPage--;
                    int newPageStart = currentPage * filesPerPage;
                    int filesOnPrevPage = std::min(filesPerPage, (int)files.size() - newPageStart);

                    // Try to maintain row position in rightmost column of previous page
                    int prevPageCols = (filesOnPrevPage + maxFilesPerColumn - 1) / maxFilesPerColumn;
                    int targetCol = prevPageCols - 1;
                    int targetIndex = newPageStart + (targetCol * maxFilesPerColumn) + currentRow;

                    // Make sure we don't go past the last file on the page
                    if (targetIndex >= newPageStart + filesOnPrevPage) {
                        targetIndex = newPageStart + filesOnPrevPage - 1;
                    }

                    selectedIndex = targetIndex;
                } else if (selectedIndex == 0 && currentPage == 0 && currentPath != BASE_DIR) {
                    // At first item on first page in a subfolder - go back to parent directory
                    size_t lastSlash = currentPath.find_last_of('/');
                    if (lastSlash != std::string::npos) {
                        currentPath = currentPath.substr(0, lastSlash);
                        selectedIndex = 0;
                        currentPage = 0;
                        statusMessage = TEXT_MSG_RETURNED;
                    }
                }
                break;
            }

            case KEY_RIGHT: {
                // Calculate current position
                int pageStartIndex = currentPage * filesPerPage;
                int pageEndIndex = std::min((int)files.size(), pageStartIndex + filesPerPage);
                int filesOnPage = pageEndIndex - pageStartIndex;
                int relativeIndex = selectedIndex - pageStartIndex;
                int currentCol = relativeIndex / maxFilesPerColumn;
                int currentRow = relativeIndex % maxFilesPerColumn;

                // Check if there's another column on this page
                if ((currentCol + 1) * maxFilesPerColumn < filesOnPage) {
                    // Move to next column (same row if possible)
                    int targetIndex = selectedIndex + maxFilesPerColumn;
                    if (targetIndex < pageEndIndex) {
                        selectedIndex = targetIndex;
                    } else {
                        // Jump to last file in next column
                        selectedIndex = pageEndIndex - 1;
                    }
                } else if (currentPage < totalPages - 1) {
                    // Move to next page, leftmost column
                    currentPage++;
                    int newPageStart = currentPage * filesPerPage;
                    int targetIndex = newPageStart + currentRow;

                    // Make sure we don't go past the last file
                    if (targetIndex >= files.size()) {
                        targetIndex = files.size() - 1;
                    }

                    selectedIndex = targetIndex;
                }
                break;
            }

            case '\n':
            case KEY_ENTER: // Enter key
                if (!files.empty()) {
                    std::string selected = files[selectedIndex];
                    // Check if it's a directory
                    if (selected.substr(0, 2) == TEXT_FOLDER_LABEL) {
                        std::string folderName = selected.substr(2);
                        currentPath = currentPath + "/" + folderName;
                        selectedIndex = 0; // Reset selection when entering folder
                        currentPage = 0; // Reset page when entering folder
                        statusMessage = std::string(TEXT_MSG_OPENED_FOLDER) + folderName;
                    } else {
                        // Check if it's a .docx file
                        std::string filepath = currentPath + "/" + selected;
                        if (selected.length() > 5 && selected.substr(selected.length() - 5) == ".docx") {
                            // Open .docx files with system default application
                            def_prog_mode();
                            endwin();

                            std::string command = "open \"" + filepath + "\"";
                            system(command.c_str());

                            reset_prog_mode();
                            refresh();
                            statusMessage = std::string(TEXT_MSG_OPENED) + selected;
                        } else {
                            // Open other files in micro editor
                            openFileInEditor(selected);
                            statusMessage = std::string(TEXT_MSG_EDITED_FILE) + selected;
                            // Sync to cloud after editing file
                            syncToCloud();
                        }
                    }
                }
                break;

            case KEY_BACK:
            case KEY_BACK - 32: // Handle uppercase
                // Go back to parent directory
                if (currentPath != BASE_DIR) {
                    size_t lastSlash = currentPath.find_last_of('/');
                    if (lastSlash != std::string::npos) {
                        currentPath = currentPath.substr(0, lastSlash);
                        selectedIndex = 0;
                        currentPage = 0;
                        statusMessage = TEXT_MSG_RETURNED;
                    }
                }
                break;

            case KEY_NEW_FILE:
            case KEY_NEW_FILE - 32: { // Handle uppercase
                std::string filename = getUserInput(TEXT_PROMPT_NEW_FILE);
                if (!filename.empty()) {
                    // Get available templates
                    std::vector<std::string> templates = getTemplates();

                    // Let user select template
                    int templateIndex = selectTemplate(templates);

                    if (templateIndex == -1) {
                        statusMessage = TEXT_MSG_USER_CANCELLED;
                    } else {
                        // If MLA format is selected and filename has no extension, add .txt
                        if (templates[templateIndex] == "Mla Format") {
                            // Check if filename already has an extension
                            size_t dotPos = filename.find_last_of('.');
                            if (dotPos == std::string::npos || dotPos == 0) {
                                // No extension found, add .txt
                                filename += ".txt";
                            }
                        }

                        // Check if file exists after potentially adding extension
                        if (fileExists(filename)) {
                            statusMessage = TEXT_MSG_ALREADY_EXISTS;
                        } else {
                            // Create file with selected template
                            createFileWithTemplate(filename, templates[templateIndex]);
                            // Record undo action
                            g_lastAction = {UNDO_CREATE_FILE, currentPath + "/" + filename, "", "", false, {}, {}};
                            statusMessage = std::string(TEXT_MSG_CREATED_FILE) + filename;
                            // Sync to cloud after creating file
                            syncToCloud();
                        }
                    }
                } else {
                    statusMessage = TEXT_MSG_USER_CANCELLED;
                }
                break;
            }

            case KEY_NEW_FOLDER:
            case KEY_NEW_FOLDER - 32: { // Handle uppercase
                std::string foldername = getUserInput(TEXT_PROMPT_NEW_FOLDER);
                if (!foldername.empty()) {
                    if (fileExists(foldername)) {
                        statusMessage = TEXT_MSG_ALREADY_EXISTS;
                    } else {
                        createFolder(foldername);
                        // Record undo action
                        g_lastAction = {UNDO_CREATE_FOLDER, currentPath + "/" + foldername, "", "", false, {}, {}};
                        statusMessage = std::string(TEXT_MSG_CREATED_FOLDER) + foldername;
                        // Sync to cloud after creating folder
                        syncToCloud();
                    }
                } else {
                    statusMessage = TEXT_MSG_USER_CANCELLED;
                }
                break;
            }

            case KEY_RENAME:
            case KEY_RENAME - 32: { // Handle uppercase
                if (!files.empty()) {
                    // Check if file is locked
                    if (isFileLockedByDisplay(files[selectedIndex])) {
                        statusMessage = TEXT_MSG_CANNOT_RENAME_LOCKED;
                        break;
                    }

                    std::string oldName = files[selectedIndex];
                    char prompt[256];
                    snprintf(prompt, sizeof(prompt), TEXT_PROMPT_RENAME, oldName.c_str());
                    std::string newName = getUserInput(prompt);
                    if (!newName.empty() && newName != oldName) {
                        // Get the actual old name (remove [] prefix if directory)
                        std::string actualOldName = oldName;
                        if (oldName.substr(0, 2) == TEXT_FOLDER_LABEL) {
                            actualOldName = oldName.substr(2);
                        }
                        std::string oldPath = currentPath + "/" + actualOldName;
                        std::string newPath = currentPath + "/" + newName;

                        renameFile(oldName, newName);
                        // Record undo action
                        g_lastAction = {UNDO_RENAME, oldPath, newPath, "", false, {}, {}};
                        statusMessage = std::string(TEXT_MSG_RENAMED) + oldName + " -> " + newName;
                        // Sync to cloud after renaming
                        syncToCloud();
                    } else {
                        statusMessage = TEXT_MSG_RENAME_CANCELLED;
                    }
                } else {
                    statusMessage = TEXT_MSG_NO_FILES_RENAME;
                }
                break;
            }

            case KEY_DC: // Delete key (ncurses constant, equals 330)
            case 383:    // Delete key on macOS/some systems
            case KEY_DELETE_ALT: // 'd' key as alternative
            case KEY_DELETE_ALT - 32: { // Handle uppercase 'D'
                // Check if we have selected items to delete
                if (!g_selectedPaths.empty()) {
                    // Check if any selected files are locked
                    bool hasLockedFiles = false;
                    for (const auto& path : g_selectedPaths) {
                        std::string filename = fs::path(path).filename().string();
                        if (isFileLocked(currentPath, filename)) {
                            hasLockedFiles = true;
                            break;
                        }
                    }

                    if (hasLockedFiles) {
                        statusMessage = TEXT_MSG_CANNOT_DELETE_LOCKED;
                        break;
                    }

                    // Bulk delete selected items
                    int itemCount = g_selectedPaths.size();
                    char deletePrompt[100];
                    snprintf(deletePrompt, sizeof(deletePrompt), "scrap %d item(s)? (y/n): ", itemCount);

                    mvprintw(LINES - 5, 0, "%s", deletePrompt);
                    clrtoeol();
                    refresh();

                    int confirm = getch();
                    // Accept 'y', 'Y', or Enter as confirmation
                    if (confirm == 'y' || confirm == 'Y' || confirm == '\n' || confirm == KEY_ENTER || confirm == 10) {
                        int deletedCount = 0;
                        int failedCount = 0;

                        // Copy selected paths to vector for iteration (avoid modifying set during iteration)
                        std::vector<std::string> pathsToDelete(g_selectedPaths.begin(), g_selectedPaths.end());

                        for (const auto& fullPath : pathsToDelete) {
                            try {
                                if (fs::exists(fullPath)) {
                                    fs::remove_all(fullPath);
                                    deletedCount++;
                                }
                            } catch (...) {
                                failedCount++;
                            }
                        }

                        // Clear selection after deletion
                        g_selectedPaths.clear();
                        g_selectMode = false;
                        g_anchorIndex = -1;

                        // Build status message
                        statusMessage = "Scrapped " + std::to_string(deletedCount) + " item(s)";
                        if (failedCount > 0) {
                            statusMessage += " (" + std::to_string(failedCount) + " failed)";
                        }

                        // Sync to cloud after bulk deletion
                        syncToCloud();
                    } else {
                        statusMessage = TEXT_MSG_DELETE_CANCELLED;
                    }
                } else if (!files.empty()) {
                    // Single item delete (no selection)
                    std::string filename = files[selectedIndex];

                    // Check if file is locked
                    if (isFileLockedByDisplay(filename)) {
                        statusMessage = TEXT_MSG_CANNOT_DELETE_LOCKED;
                        break;
                    }

                    // Replace separator line with delete prompt
                    mvprintw(LINES - 5, 0, TEXT_PROMPT_DELETE_CONFIRM, filename.c_str());
                    clrtoeol();
                    refresh();

                    int confirm = getch();
                    // Accept 'y', 'Y', or Enter as confirmation
                    if (confirm == 'y' || confirm == 'Y' || confirm == '\n' || confirm == KEY_ENTER || confirm == 10) {
                        // Get actual filename (remove [] prefix if directory)
                        std::string actualName = filename;
                        bool isDir = false;
                        if (filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
                            actualName = filename.substr(2);
                            isDir = true;
                        }
                        std::string fullPath = currentPath + "/" + actualName;

                        // Save content if it's a file (for undo)
                        std::string content = "";
                        if (!isDir && fs::exists(fullPath)) {
                            std::ifstream inFile(fullPath);
                            if (inFile.is_open()) {
                                std::string line;
                                while (std::getline(inFile, line)) {
                                    content += line + "\n";
                                }
                                inFile.close();
                            }
                        }

                        deleteFile(filename);
                        // Record undo action
                        g_lastAction = {UNDO_DELETE, fullPath, "", content, isDir, {}, {}};
                        statusMessage = std::string(TEXT_MSG_DELETED) + filename;
                        // Sync to cloud after deleting
                        syncToCloud();
                        if (selectedIndex >= files.size() - 1 && selectedIndex > 0) {
                            selectedIndex--;
                        }
                    } else {
                        statusMessage = TEXT_MSG_DELETE_CANCELLED;
                    }
                } else {
                    statusMessage = TEXT_MSG_NO_FILES_DELETE;
                }
                break;
            }

            case KEY_CONVERT:
            case KEY_CONVERT - 32: { // Handle uppercase 'C'
                if (!files.empty()) {
                    convertMarkdownToDocx(files[selectedIndex], statusMessage);
                } else {
                    statusMessage = TEXT_MSG_NO_FILE_FOR_CONVERSION;
                }
                break;
            }

            case KEY_LOCK:
            case KEY_LOCK - 32: { // Handle uppercase 'L'
                if (!files.empty()) {
                    if (g_selectedPaths.empty()) {
                        // Lock/unlock single file
                        toggleFileLock(files[selectedIndex], statusMessage);
                    } else {
                        // Lock/unlock multiple selected files
                        int lockCount = 0;
                        for (const auto& path : g_selectedPaths) {
                            std::string filename = fs::path(path).filename().string();
                            std::string parentDir = fs::path(path).parent_path().string();

                            // Load locks for the parent directory
                            auto locks = loadLocks(parentDir);

                            // Toggle lock
                            if (locks.find(filename) != locks.end()) {
                                locks.erase(filename);
                            } else {
                                locks.insert(filename);
                            }

                            saveLocks(parentDir, locks);
                            lockCount++;
                        }
                        statusMessage = "Toggled lock on " + std::to_string(lockCount) + " item(s)";
                        g_selectedPaths.clear();
                        g_selectMode = false;
                    }
                }
                break;
            }

            case KEY_SETTINGS:
            case KEY_SETTINGS - 32: { // Handle uppercase 'T'
                showSettingsMenu();
                statusMessage = TEXT_MSG_IDLE;
                break;
            }

            case KEY_QUIT:
            case KEY_QUIT - 32: // Handle uppercase
                running = false;
                break;

            // 'f' or 'F' - enter search/find mode
            case KEY_SEARCH:
            case KEY_SEARCH - 32: { // Handle uppercase 'F'
                enterSearchMode(files, selectedIndex, statusMessage);
                break;
            }

            // 's' or 'S' - toggle selection mode
            case 's':
            case 'S': {
                if (!files.empty()) {
                    std::string fullPath = getFullPath(files[selectedIndex]);

                    if (!g_selectMode) {
                        // Turn ON selection mode
                        g_selectMode = true;
                        g_anchorIndex = selectedIndex;
                        g_anchorPath = fullPath;

                        // Select the current item
                        g_selectedPaths.insert(fullPath);

                        statusMessage = TEXT_MSG_SELECTION_MODE_ON;
                    } else {
                        // Turn OFF selection mode (lock in selections)
                        g_selectMode = false;
                        g_anchorIndex = -1;
                        g_anchorPath = "";

                        statusMessage = std::string(TEXT_MSG_SELECTION_LOCKED) + std::to_string(g_selectedPaths.size()) + " item(s))";
                    }
                }
                break;
            }

            case 27: // ESC key -> cancel selection
                if (!g_selectedPaths.empty() || g_selectMode) {
                    g_selectedPaths.clear();
                    g_selectMode = false;
                    g_anchorIndex = -1;
                    g_anchorPath = "";
                    statusMessage = TEXT_MSG_SELECTION_CANCELLED;
                }
                break;

            case 'v':
            case 'V': {
                // Move selected items to current directory
                if (!g_selectedPaths.empty()) {
                    // Check if any selected files are locked
                    bool hasLockedFiles = false;
                    for (const auto& path : g_selectedPaths) {
                        std::string filename = fs::path(path).filename().string();
                        // Extract the parent directory from the path
                        std::string parentDir = fs::path(path).parent_path().string();
                        if (isFileLocked(parentDir, filename)) {
                            hasLockedFiles = true;
                            break;
                        }
                    }

                    if (hasLockedFiles) {
                        statusMessage = TEXT_MSG_CANNOT_MOVE_LOCKED;
                        g_selectedPaths.clear();
                        g_selectMode = false;
                        break;
                    }

                    int moveCount = 0;
                    int errorCount = 0;
                    std::vector<std::string> pathsToMove(g_selectedPaths.begin(), g_selectedPaths.end());
                    std::vector<std::string> movedSources;
                    std::vector<std::string> movedDests;

                    for (const std::string& sourcePath : pathsToMove) {
                        try {
                            // Extract filename from source path
                            size_t lastSlash = sourcePath.find_last_of('/');
                            std::string filename = (lastSlash != std::string::npos)
                                ? sourcePath.substr(lastSlash + 1)
                                : sourcePath;

                            // Don't move if source and destination are the same
                            std::string destPath = currentPath + "/" + filename;
                            if (sourcePath == destPath) {
                                continue; // Skip items already in target folder
                            }

                            // Check if destination already exists
                            if (fs::exists(destPath)) {
                                errorCount++;
                                continue;
                            }

                            // Move the file/folder
                            fs::rename(sourcePath, destPath);
                            moveCount++;

                            // Track for undo
                            movedSources.push_back(sourcePath);
                            movedDests.push_back(destPath);

                            // Remove from selection after successful move
                            g_selectedPaths.erase(sourcePath);
                        } catch (...) {
                            errorCount++;
                        }
                    }

                    if (moveCount > 0) {
                        // Record undo action
                        g_lastAction = {UNDO_MOVE, "", "", "", false, movedSources, movedDests};
                        statusMessage = std::string(TEXT_MSG_MOVED_ITEMS) + std::to_string(moveCount) + " item(s)";
                        // Sync to cloud after moving items
                        syncToCloud();
                        if (errorCount > 0) {
                            statusMessage += " (" + std::to_string(errorCount) + TEXT_MSG_MOVE_FAILED;
                        }
                    } else if (errorCount > 0) {
                        statusMessage = TEXT_MSG_MOVE_ERROR;
                    } else {
                        statusMessage = TEXT_MSG_NO_ITEMS_MOVE;
                    }

                    // Clear selection if all items were moved
                    if (g_selectedPaths.empty()) {
                        g_selectMode = false;
                        g_anchorIndex = -1;
                        g_anchorPath = "";
                    }
                } else {
                    statusMessage = TEXT_MSG_NO_ITEMS_TO_MOVE;
                }
                break;
            }

            case KEY_UNDO_ACTION:
            case KEY_UNDO_ACTION - 32: { // Handle uppercase 'Z'
                performUndo(statusMessage);
                break;
            }
        }
    }

    // Clean up ncurses
    endwin();

    return 0;
}
