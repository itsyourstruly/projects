#include "file_operations.h"
#include "config.h"
#include "text.h"
#include "lock_system.h"
#include "cloud_sync.h"
#include <ncurses.h>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

void ensureDirectories() {
    if (!fs::exists(BASE_DIR)) {
        fs::create_directory(BASE_DIR);
    }
    if (!fs::exists(TEMPLATES_DIR)) {
        fs::create_directory(TEMPLATES_DIR);
    }
}

std::vector<std::string> getFiles() {
    std::vector<std::string> files;

    if (fs::exists(currentPath) && fs::is_directory(currentPath)) {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();

                // Skip hidden lock file
                if (filename == ".jot_locks") {
                    continue;
                }

                // Check if filename starts with {S} to indicate smart note
                if (filename.length() >= 3 && filename.substr(0, 3) == "{S}") {
                    files.push_back(TEXT_SMART_NOTE_LABEL + filename.substr(3));
                } else {
                    files.push_back(filename);
                }
            } else if (entry.is_directory()) {
                files.push_back(TEXT_FOLDER_LABEL + entry.path().filename().string());
            }
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::string> getTemplates() {
    std::vector<std::string> templates;
    templates.push_back(TEXT_MSG_BLANK_LABEL);

    if (fs::exists(TEMPLATES_DIR) && fs::is_directory(TEMPLATES_DIR)) {
        for (const auto& entry : fs::directory_iterator(TEMPLATES_DIR)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();

                if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".txt") {
                    filename = filename.substr(0, filename.size() - 4);

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

std::vector<std::string> getAllFilesRecursive(const std::string& basePath) {
    std::vector<std::string> allFiles;

    if (!fs::exists(basePath) || !fs::is_directory(basePath)) {
        return allFiles;
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(basePath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();

                // Skip hidden lock file
                if (filename == ".jot_locks") {
                    continue;
                }

                // Get relative path from base
                std::string relativePath = fs::relative(entry.path(), basePath).string();

                // Format the path for display
                std::string displayPath = relativePath;

                // Check if filename starts with {S} to indicate smart note
                if (filename.length() >= 3 && filename.substr(0, 3) == "{S}") {
                    // Replace the filename part with smart note indicator
                    size_t lastSlash = displayPath.find_last_of('/');
                    if (lastSlash != std::string::npos) {
                        displayPath = displayPath.substr(0, lastSlash + 1) + TEXT_SMART_NOTE_LABEL + filename.substr(3);
                    } else {
                        displayPath = TEXT_SMART_NOTE_LABEL + filename.substr(3);
                    }
                }

                allFiles.push_back(displayPath);
            } else if (entry.is_directory()) {
                std::string relativePath = fs::relative(entry.path(), basePath).string();
                allFiles.push_back(TEXT_FOLDER_LABEL + relativePath);
            }
        }
    } catch (const fs::filesystem_error& e) {
        // Handle permission errors or other filesystem issues
    }

    std::sort(allFiles.begin(), allFiles.end());
    return allFiles;
}

std::string getFullPath(const std::string& filename) {
    std::string actualName = filename;
    if (filename.substr(0, 2) == "[]") {
        actualName = filename.substr(2);
    }
    return currentPath + "/" + actualName;
}

std::string getActualName(const std::string& displayName) {
    std::string name = displayName;

    if (name.length() >= 2 && name.substr(0, 2) == TEXT_FOLDER_LABEL) {
        name = name.substr(2);
    }

    if (name.length() >= 3 && name.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        return "{S}" + name.substr(3);
    }

    return name;
}

std::string getActualFilename(const std::string& displayName) {
    std::string name = displayName;

    if (name.length() >= 2 && name.substr(0, 2) == TEXT_FOLDER_LABEL) {
        name = name.substr(2);
    }

    if (name.length() >= 3 && name.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        return "{S}" + name.substr(3);
    }

    return name;
}

bool fileExists(const std::string& name) {
    std::string path = currentPath + "/" + name;
    return fs::exists(path);
}

void createFolder(const std::string& name) {
    std::string path = currentPath + "/" + name;
    fs::create_directory(path);
}

void deleteFile(const std::string& filename) {
    std::string actualName = filename;
    if (filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        actualName = filename.substr(2);
    }
    if (filename.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        actualName = "{S}" + filename.substr(3);
    }
    std::string path = currentPath + "/" + actualName;
    fs::remove_all(path);
}

void renameFile(const std::string& oldName, const std::string& newName) {
    std::string actualOldName = oldName;
    if (oldName.substr(0, 2) == TEXT_FOLDER_LABEL) {
        actualOldName = oldName.substr(2);
    }
    if (oldName.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        actualOldName = "{S}" + oldName.substr(3);
    }

    std::string oldPath = currentPath + "/" + actualOldName;
    std::string newPath = currentPath + "/" + newName;
    fs::rename(oldPath, newPath);
}

std::string getTimeAgo(const std::string& filepath) {
    if (!fs::exists(filepath)) {
        return "";
    }

    auto ftime = fs::last_write_time(filepath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    auto time = std::chrono::system_clock::to_time_t(sctp);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    auto diff = now - time;

    if (diff < 60) {
        return "just now";
    } else if (diff < 3600) {
        int mins = diff / 60;
        return std::to_string(mins) + " minute" + (mins > 1 ? "s" : "") + " ago";
    } else if (diff < 86400) {
        int hours = diff / 3600;
        return std::to_string(hours) + " hour" + (hours > 1 ? "s" : "") + " ago";
    } else {
        int days = diff / 86400;
        return std::to_string(days) + " day" + (days > 1 ? "s" : "") + " ago";
    }
}

void toggleFileLock(const std::string& displayFilename, std::string& statusMessage) {
    auto locks = loadLocks(currentPath);
    std::string actualName = getActualFilename(displayFilename);

    if (locks.find(actualName) != locks.end()) {
        locks.erase(actualName);
        statusMessage = std::string(TEXT_MSG_UNLOCKED) + actualName;
    } else {
        locks.insert(actualName);
        statusMessage = std::string(TEXT_MSG_LOCKED) + actualName;
    }

    saveLocks(currentPath, locks);
}

bool isFileLockedByDisplay(const std::string& displayFilename) {
    std::string actualName = getActualFilename(displayFilename);
    return isFileLocked(currentPath, actualName);
}

void createFileWithTemplate(const std::string& filename, const std::string& templateName) {
    // Check if this is a smart template
    bool isSmartTemplate = (templateName == "Smart");

    // Add {S} prefix to filename if it's a smart note
    std::string actualFilename = filename;
    if (isSmartTemplate && actualFilename.substr(0, 3) != "{S}") {
        actualFilename = "{S}" + actualFilename;
    }

    std::string filepath = currentPath + "/" + actualFilename;

    // Create and open file with explicit flags
    std::ofstream outFile(filepath, std::ios::out | std::ios::trunc);

    if (!outFile.is_open() || !outFile.good()) {
        // File creation failed
        return;
    }

    if (templateName == TEXT_MSG_BLANK_LABEL) {
        // Create empty file - write nothing, just open and close
        // No content needed for blank file
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

    // Explicitly flush and close
    outFile.flush();
    outFile.close();
}

// Function to create a new folder
void convertMarkdownToDocx(const std::string& filename, std::string& statusMessage) {
    std::string actualFilename = filename;

    // Remove [] prefix if somehow present
    if (actualFilename.length() >= 2 && actualFilename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        statusMessage = TEXT_MSG_CANNOT_CONVERT_DIR;
        return;
    }

    // Remove {S} prefix if present
    if (actualFilename.length() >= 3 && actualFilename.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        actualFilename = "{S}" + actualFilename.substr(3);
    }

    // Check if file is markdown or text
    std::string ext = "";
    size_t dotPos = actualFilename.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = actualFilename.substr(dotPos);
    }

    if (ext != ".md" && ext != ".txt") {
        statusMessage = TEXT_MSG_FILE_MUST_BE_MD_TXT;
        return;
    }

    std::string filepath = currentPath + "/" + actualFilename;
    std::string outputFile = filepath.substr(0, filepath.length() - ext.length()) + ".docx";

    // Show converting message
    mvprintw(LINES - 7, 0, TEXT_MSG_CONVERTING);
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
        statusMessage = std::string(TEXT_MSG_CONVERTED_TO) + outputFilename + TEXT_MSG_MLA_APPLIED;
    } else {
        statusMessage = TEXT_MSG_CONVERSION_FAILED;
    }
}

void openFileInEditor(const std::string& filename) {
    // Don't try to edit directories
    if (filename.length() >= 2 && filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        return;
    }

    // Check if this is a smart note by looking for {S} prefix
    bool isSmartNote = (filename.length() >= 3 && filename.substr(0, 3) == TEXT_SMART_NOTE_LABEL);

    // Remove {S} prefix to get actual filename
    std::string actualFilename = filename;
    if (isSmartNote) {
        actualFilename = filename.substr(3); // Remove {S} prefix
    }

    std::string filepath = currentPath + "/" + (isSmartNote ? "{S}" : "") + actualFilename;

    // Suspend ncurses temporarily
    def_prog_mode(); // Save current terminal state
    endwin(); // End ncurses mode

    // Launch micro editor with softwrap and spellcheck enabled
    std::string command = "micro -softwrap true \"" + filepath + "\"";
    system(command.c_str());

    // If this is a smart note, format it after editing
    if (isSmartNote) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::string scriptPath = std::string(cwd) + "/templates/smart_format.py";
        std::string formatCmd = "python3 \"" + scriptPath + "\" \"" + filepath + "\" \"" + filepath + ".tmp\"";
        int result = system(formatCmd.c_str());

        if (result == 0) {
            fs::rename(filepath + ".tmp", filepath);
        }
    }

    // Auto-sync to cloud if enabled
    if (g_cloudSyncEnabled && g_autoSyncEnabled) {
        syncToCloud(true);
    }

    // Resume ncurses
    reset_prog_mode(); // Restore terminal state
    refresh(); // Refresh the screen
}

