#include <ncurses.h>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <unistd.h>

namespace fs = std::filesystem;

// Directory configuration
const std::string BASE_DIR = "files";
const std::string TEMPLATES_DIR = "templates";
std::string currentPath = BASE_DIR;

// UI Layout configuration
const int COLUMN_WIDTH = 30; // Width of each file column
const int MAX_COLUMNS = 3; // Maximum columns per page
const int TOP_OFFSET = 3; // Lines before file list starts
const int BOTTOM_OFFSET = 5; // Lines for footer controls

// Key bindings - easy to customize
const char KEY_NEW_FILE = 'n';
const char KEY_NEW_FOLDER = 'f';
const char KEY_RENAME = 'r';
const char KEY_DELETE_ALT = 'd'; // Alternative delete key
const char KEY_BACK = 'b';
const char KEY_CONVERT = 'c'; // Convert markdown to Word
const char KEY_QUIT = 'q';
// Primary delete uses the actual Delete key (KEY_DC in ncurses)

// UI Text - easy to customize
const char* TEXT_HEADER = "Happy Writing!";
const char* TEXT_SEPARATOR = "----------------------------------------";
const char* TEXT_HOME_FOLDER = "You are in the home folder";
const char* TEXT_YOU_ARE_IN = "You are in the '%s' folder";
const char* TEXT_PAGE_INDICATOR = "Page %d/%d";
const char* TEXT_NO_FILES = "(No files - press 'n' to create one)";
const char* TEXT_CONTROLS = "Controls:";

// Control labels
const char* TEXT_ARROWS = "Arrows: Navigate";
const char* TEXT_ENTER = "ENTER: Open";
const char* TEXT_NEW_FILE_LABEL = "New File";
const char* TEXT_NEW_FOLDER_LABEL = "New Folder";
const char* TEXT_RENAME_LABEL = "Rename";
const char* TEXT_DELETE_LABEL = "Delete";
const char* TEXT_DELETE_KEY_LABEL = "DEL"; // Display name for delete key
const char* TEXT_CONVERT_LABEL = "Convert";
const char* TEXT_BACK_LABEL = "Back";
const char* TEXT_QUIT_LABEL = "Quit";

// Prompt messages
const char* TEXT_PROMPT_NEW_FILE = "Enter new filename (press ESC to cancel): ";
const char* TEXT_PROMPT_NEW_FOLDER = "Enter new folder name (press ESC to cancel): ";
const char* TEXT_PROMPT_RENAME = "Enter new name for '%s' (press ESC to cancel): ";
const char* TEXT_PROMPT_DELETE_CONFIRM = "Delete '%s'? (y/n): ";
const char* TEXT_PROMPT_SELECT_TEMPLATE = "Select a template (use arrow keys, ENTER to select, ESC to cancel):";

// Status messages
const char* TEXT_MSG_IDLE = "working";
const char* TEXT_MSG_OPENED_FOLDER = "Opened folder: ";
const char* TEXT_MSG_EDITED_FILE = "Edited file: ";
const char* TEXT_MSG_RETURNED = "Returned to parent folder";
const char* TEXT_MSG_CREATED_FILE = "Created file: ";
const char* TEXT_MSG_CREATED_FOLDER = "Created folder: ";
const char* TEXT_MSG_USER_CANCELLED = "Cancelled by user.";
const char* TEXT_MSG_ALREADY_EXISTS = "File/folder name already exists.";
const char* TEXT_MSG_RENAMED = "Renamed: ";
const char* TEXT_MSG_RENAME_CANCELLED = "Rename cancelled.";
const char* TEXT_MSG_NO_FILES_RENAME = "No files to rename.";
const char* TEXT_MSG_DELETED = "Deleted: ";
const char* TEXT_MSG_DELETE_CANCELLED = "Delete cancelled.";
const char* TEXT_MSG_NO_FILES_DELETE = "No files to delete.";

// Function to ensure the files and templates directories exist
void ensureDirectories() {
    if (!fs::exists(BASE_DIR)) {
        fs::create_directory(BASE_DIR);
    }
    if (!fs::exists(TEMPLATES_DIR)) {
        fs::create_directory(TEMPLATES_DIR);
    }
}

// Function to get all files and folders in the files directory
std::vector<std::string> getFiles() {
    std::vector<std::string> files;

    if (fs::exists(currentPath) && fs::is_directory(currentPath)) {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            } else if (entry.is_directory()) {
                files.push_back("[DIR] " + entry.path().filename().string());
            }
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

// Function to get available templates
std::vector<std::string> getTemplates() {
    std::vector<std::string> templates;
    templates.push_back("Blank"); // Always include blank option first

    if (fs::exists(TEMPLATES_DIR) && fs::is_directory(TEMPLATES_DIR)) {
        for (const auto& entry : fs::directory_iterator(TEMPLATES_DIR)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();

                // ONLY process .txt files (skip .py, .lua, etc.)
                if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".txt") {
                    // Remove .txt extension for display
                    filename = filename.substr(0, filename.size() - 4);

                    // Format name: replace underscores with spaces and capitalize
                    for (size_t i = 0; i < filename.length(); i++) {
                        if (filename[i] == '_') {
                            filename[i] = ' ';
                        }
                        if (i == 0 || filename[i-1] == ' ') {
                            filename[i] = toupper(filename[i]);
                        }
                    }
                    templates.push_back(filename);
                }
            }
        }
    }

    return templates;
}

// Function to draw the UI
void drawUI(const std::vector<std::string>& files, int selectedIndex, int currentPage, const std::string& message = "") {
    clear();

    // Draw header with status on the same line
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_HEADER);

    // Display status on the right side of the same line
    std::string statusText = message.empty() ? TEXT_MSG_IDLE : message;
    std::string statusDisplay = std::string("Status: ") + statusText;
    int statusX = COLS - statusDisplay.length() - 2; // 2 chars padding from right edge
    if (statusX < strlen(TEXT_HEADER) + 5) { // Ensure it doesn't overlap with header
        statusX = strlen(TEXT_HEADER) + 5;
    }
    mvprintw(0, statusX, "%s", statusDisplay.c_str());
    attroff(A_BOLD);

    // Show current location
    if (currentPath == BASE_DIR) {
        mvprintw(1, 0, TEXT_HOME_FOLDER);
    } else {
        // Extract folder name from path
        std::string folderName = currentPath.substr(BASE_DIR.length() + 1);
        mvprintw(1, 0, TEXT_YOU_ARE_IN, folderName.c_str());
    }

    // Calculate pagination
    int maxFilesPerColumn = LINES - TOP_OFFSET - BOTTOM_OFFSET;
    int filesPerPage = maxFilesPerColumn * MAX_COLUMNS;
    int totalPages = files.empty() ? 1 : (files.size() + filesPerPage - 1) / filesPerPage;

    // Display page indicator if more than one page
    if (totalPages > 1) {
        char pageIndicator[50];
        snprintf(pageIndicator, sizeof(pageIndicator), TEXT_PAGE_INDICATOR, currentPage + 1, totalPages);
        int pageX = COLS - strlen(pageIndicator) - 2;
        mvprintw(1, pageX, "%s", pageIndicator);
    }

    mvprintw(2, 0, TEXT_SEPARATOR);

    int startY = TOP_OFFSET;

    // Draw files in columns for current page
    if (files.empty()) {
        mvprintw(startY, 2, TEXT_NO_FILES);
    } else {
        // Calculate which files to display on this page
        int pageStartIndex = currentPage * filesPerPage;
        int pageEndIndex = std::min((int)files.size(), pageStartIndex + filesPerPage);

        for (int i = pageStartIndex; i < pageEndIndex; i++) {
            // Calculate position relative to page start
            int relativeIndex = i - pageStartIndex;
            int col = relativeIndex / maxFilesPerColumn;
            int row = relativeIndex % maxFilesPerColumn;

            // Calculate x position (with some left padding)
            int xPos = 2 + (col * COLUMN_WIDTH);
            int yPos = startY + row;

            // Only draw if within column limit
            if (col < MAX_COLUMNS) {
                if (i == selectedIndex) {
                    attron(A_REVERSE); // Highlight selected file
                }

                // Truncate filename if too long for column
                std::string displayName = files[i];
                if (displayName.length() > COLUMN_WIDTH - 4) {
                    displayName = displayName.substr(0, COLUMN_WIDTH - 7) + "...";
                }

                mvprintw(yPos, xPos, "%s", displayName.c_str());

                if (i == selectedIndex) {
                    attroff(A_REVERSE);
                }
            }
        }
    }

    // Draw footer with instructions
    int footerY = LINES - 5;
    mvprintw(footerY, 0, TEXT_SEPARATOR);
    mvprintw(footerY + 1, 0, TEXT_CONTROLS);

    if (currentPath == BASE_DIR) {
        mvprintw(footerY + 2, 0, "  %s | %s | %c: %s | %c: %s | %c: %s | %s/%c: %s | %c: %s | %c: %s",
                 TEXT_ARROWS, TEXT_ENTER, KEY_NEW_FILE, TEXT_NEW_FILE_LABEL,
                 KEY_NEW_FOLDER, TEXT_NEW_FOLDER_LABEL, KEY_RENAME, TEXT_RENAME_LABEL,
                 TEXT_DELETE_KEY_LABEL, KEY_DELETE_ALT, TEXT_DELETE_LABEL,
                 KEY_CONVERT, TEXT_CONVERT_LABEL, KEY_QUIT, TEXT_QUIT_LABEL);
    } else {
        mvprintw(footerY + 2, 0, "  %s | %s | %c: %s | %c: %s | %c: %s | %c: %s | %s/%c: %s | %c: %s | %c: %s",
                 TEXT_ARROWS, TEXT_ENTER, KEY_BACK, TEXT_BACK_LABEL,
                 KEY_NEW_FILE, TEXT_NEW_FILE_LABEL, KEY_NEW_FOLDER, TEXT_NEW_FOLDER_LABEL,
                 KEY_RENAME, TEXT_RENAME_LABEL, TEXT_DELETE_KEY_LABEL, KEY_DELETE_ALT, TEXT_DELETE_LABEL,
                 KEY_CONVERT, TEXT_CONVERT_LABEL, KEY_QUIT, TEXT_QUIT_LABEL);
    }


    refresh();
}

// Function to get user input (string) with cancel support
std::string getUserInput(const std::string& prompt) {
    int y = LINES - 7;
    mvprintw(y, 0, "%s", prompt.c_str());
    clrtoeol();
    refresh();

    curs_set(1); // Show cursor during input
    noecho(); // Don't auto-echo, we'll handle it manually

    std::string input;
    int ch;

    while (true) {
        ch = getch();

        if (ch == 27) {
            // ESC key pressed - cancel
            curs_set(0);
            return "";
        } else if (ch == '\n' || ch == KEY_ENTER || ch == 10) {
            // User pressed enter
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8 || ch == 263) {
            // Handle backspace only (not forward delete)
            if (!input.empty()) {
                input.pop_back();
                // Redraw the input line
                mvprintw(y, 0, "%s", prompt.c_str());
                clrtoeol();
                if (!input.empty()) {
                    printw("%s", input.c_str());
                }
                refresh();
            }
        } else if (ch >= 32 && ch <= 126 && ch != 127) {
            // Printable character (excluding delete)
            input += (char)ch;
            addch(ch);
            refresh();
        }
    }

    curs_set(0); // Hide cursor

    return input;
}


// Function to display template selection UI
int selectTemplate(const std::vector<std::string>& templates) {
    int selectedTemplate = 0;
    bool selecting = true;

    while (selecting) {
        clear();
        mvprintw(1, 0, TEXT_PROMPT_SELECT_TEMPLATE);
        mvprintw(2, 0, TEXT_SEPARATOR);

        // Display templates
        for (size_t i = 0; i < templates.size(); i++) {
            if (i == selectedTemplate) {
                attron(A_REVERSE);
            }
            mvprintw(3 + i, 2, "%s", templates[i].c_str());
            if (i == selectedTemplate) {
                attroff(A_REVERSE);
            }
        }

        mvprintw(3 + templates.size() + 1, 0, TEXT_SEPARATOR);
        mvprintw(3 + templates.size() + 2, 0, "Use arrow keys to navigate, ENTER to select, ESC to cancel");

        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP:
                if (selectedTemplate > 0) {
                    selectedTemplate--;
                }
                break;
            case KEY_DOWN:
                if (selectedTemplate < templates.size() - 1) {
                    selectedTemplate++;
                }
                break;
            case '\n':
            case KEY_ENTER:
                return selectedTemplate;
            case 27: // ESC key
                return -1; // Cancelled
        }
    }

    return -1;
}

// Function to create file with template
void createFileWithTemplate(const std::string& filename, const std::string& templateName) {
    std::string filepath = currentPath + "/" + filename;
    std::ofstream outFile(filepath);

    if (!outFile.is_open()) {
        return;
    }

    if (templateName == "Blank") {
        // Create empty file
        outFile << "";
    } else {
        // Find and copy template content
        // Convert display name back to filename
        std::string templateFilename = templateName;
        for (size_t i = 0; i < templateFilename.length(); i++) {
            templateFilename[i] = tolower(templateFilename[i]);
            if (templateFilename[i] == ' ') {
                templateFilename[i] = '_';
            }
        }
        templateFilename += ".txt";

        std::string templatePath = TEMPLATES_DIR + "/" + templateFilename;

        // Check if template file exists
        if (fs::exists(templatePath)) {
            std::ifstream inFile(templatePath);
            if (inFile.is_open()) {
                // Copy template content to new file
                std::string line;
                while (std::getline(inFile, line)) {
                    outFile << line << "\n";
                }
                inFile.close();
            }
        }
    }

    outFile.close();
}

// Function to create a new folder
void createFolder(const std::string& foldername) {
    std::string folderpath = currentPath + "/" + foldername;
    fs::create_directory(folderpath);
}

// Function to check if a file or folder already exists
bool fileExists(const std::string& name) {
    std::string filepath = currentPath + "/" + name;
    return fs::exists(filepath);
}

// Function to delete a file or folder
void deleteFile(const std::string& filename) {
    std::string filepath = currentPath + "/" + filename;
    // Remove [DIR] prefix if present
    std::string actualName = filename;
    if (filename.substr(0, 6) == "[DIR] ") {
        actualName = filename.substr(6);
        filepath = currentPath + "/" + actualName;
    }
    fs::remove_all(filepath); // remove_all handles both files and directories
}

// Function to rename a file or folder
void renameFile(const std::string& oldName, const std::string& newName) {
    // Remove [DIR] prefix if present
    std::string actualOldName = oldName;
    if (oldName.substr(0, 6) == "[DIR] ") {
        actualOldName = oldName.substr(6);
    }

    std::string oldPath = currentPath + "/" + actualOldName;
    std::string newPath = currentPath + "/" + newName;
    fs::rename(oldPath, newPath);
}

// Function to open a file in micro editor
void openFileInEditor(const std::string& filename) {
    // Don't try to edit directories
    if (filename.substr(0, 6) == "[DIR] ") {
        return;
    }

    std::string filepath = currentPath + "/" + filename;

    // Suspend ncurses temporarily
    def_prog_mode(); // Save current terminal state
    endwin(); // End ncurses mode

    // Launch micro editor with softwrap enabled for optimal essay writing
    std::string command = "micro -softwrap true \"" + filepath + "\"";
    system(command.c_str());

    // Resume ncurses
    reset_prog_mode(); // Restore terminal state
    refresh(); // Refresh the screen
}

// Function to convert text file to MLA-formatted DOCX
void convertMarkdownToDocx(const std::string& filename, std::string& statusMessage) {
    // Remove [DIR] prefix if somehow present
    std::string actualFilename = filename;
    if (filename.substr(0, 6) == "[DIR] ") {
        statusMessage = "Cannot convert a directory";
        return;
    }

    // Check if file is markdown or text
    std::string ext = "";
    size_t dotPos = actualFilename.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = actualFilename.substr(dotPos);
    }

    if (ext != ".md" && ext != ".txt") {
        statusMessage = "File must be .md or .txt format";
        return;
    }

    std::string filepath = currentPath + "/" + actualFilename;
    std::string outputFile = filepath.substr(0, filepath.length() - ext.length()) + ".docx";

    // Show converting message
    mvprintw(LINES - 7, 0, "Converting to Word document with MLA formatting...");
    clrtoeol();
    refresh();

    // Suspend ncurses
    def_prog_mode();
    endwin();

    // Use direct Python conversion (bypasses Pandoc's paragraph merging issues)
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string scriptPath = std::string(cwd) + "/templates/convert_to_mla.py";

    std::string pythonCommand = "python3 \"" + scriptPath + "\" \"" + filepath + "\" \"" + outputFile + "\" > /tmp/mla_convert.log 2>&1";
    int result = system(pythonCommand.c_str());

    // Resume ncurses
    reset_prog_mode();
    refresh();

    if (result == 0) {
        std::string outputFilename = outputFile.substr(currentPath.length() + 1);
        statusMessage = "Converted to: " + outputFilename + " (MLA formatting applied)";
    } else {
        statusMessage = "Conversion failed - check /tmp/mla_convert.log (Ensure python-docx is installed)";
    }
}

int main() {
    // Ensure files and templates directories exist
    ensureDirectories();

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
                    selectedIndex--;
                }
                break;

            case KEY_DOWN:
                if (!files.empty() && selectedIndex < files.size() - 1) {
                    selectedIndex++;
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
                    if (selected.substr(0, 6) == "[DIR] ") {
                        std::string folderName = selected.substr(6);
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
                            statusMessage = "Opened: " + selected;
                        } else {
                            // Open other files in micro editor
                            openFileInEditor(selected);
                            statusMessage = std::string(TEXT_MSG_EDITED_FILE) + selected;
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
                            statusMessage = std::string(TEXT_MSG_CREATED_FILE) + filename;
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
                        statusMessage = std::string(TEXT_MSG_CREATED_FOLDER) + foldername;
                    }
                } else {
                    statusMessage = TEXT_MSG_USER_CANCELLED;
                }
                break;
            }

            case KEY_RENAME:
            case KEY_RENAME - 32: { // Handle uppercase
                if (!files.empty()) {
                    std::string oldName = files[selectedIndex];
                    char prompt[256];
                    snprintf(prompt, sizeof(prompt), TEXT_PROMPT_RENAME, oldName.c_str());
                    std::string newName = getUserInput(prompt);
                    if (!newName.empty() && newName != oldName) {
                        renameFile(oldName, newName);
                        statusMessage = std::string(TEXT_MSG_RENAMED) + oldName + " -> " + newName;
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
                if (!files.empty()) {
                    std::string filename = files[selectedIndex];
                    mvprintw(LINES - 7, 0, TEXT_PROMPT_DELETE_CONFIRM, filename.c_str());
                    clrtoeol();
                    refresh();

                    int confirm = getch();
                    if (confirm == 'y' || confirm == 'Y') {
                        deleteFile(filename);
                        statusMessage = std::string(TEXT_MSG_DELETED) + filename;
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
                    statusMessage = "No file selected for conversion";
                }
                break;
            }

            case KEY_QUIT:
            case KEY_QUIT - 32: // Handle uppercase
                running = false;
                break;
        }
    }

    // Clean up ncurses
    endwin();

    return 0;
}