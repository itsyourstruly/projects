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
    // Quick validation to avoid unnecessary work
    if (!fs::exists(currentPath) || !fs::is_directory(currentPath)) {
        return files;
    }
    try {
        // Reserve space to avoid reallocations (estimate 50 items)
        files.reserve(50);
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            const std::string filename = entry.path().filename().string();
            // Skip hidden lock file
            if (filename == ".jot_locks") {
                continue;
            }
            if (entry.is_regular_file()) {
                // Check if filename starts with {S} to indicate smart note
                if (filename.length() >= 3 && filename.compare(0, 3, "{S}") == 0) {
                    files.push_back(TEXT_SMART_NOTE_LABEL + filename.substr(3));
                } else {
                    files.push_back(filename);
                }
            } else if (entry.is_directory()) {
                files.push_back(TEXT_FOLDER_LABEL + filename);
            }
        }
    } catch (const fs::filesystem_error&) {
        // Handle errors gracefully, return what we have
    }
    std::sort(files.begin(), files.end());
    return files;
}
std::vector<std::string> getTemplates() {
    std::vector<std::string> templates;
    templates.push_back(TEXT_MSG_BLANK_LABEL); // Always add "empty" option first
    // Check if templates directory exists
    if (!fs::exists(TEMPLATES_DIR) || !fs::is_directory(TEMPLATES_DIR)) {
        return templates; // Return just "empty" if no templates dir
    }
    try {
        for (const auto& entry : fs::directory_iterator(TEMPLATES_DIR)) {
            // Only process regular files
            if (!entry.is_regular_file()) {
                continue;
            }
            const std::string filename = entry.path().filename().string();
            // Check for .txt extension
            if (filename.size() > 4 && filename.compare(filename.size() - 4, 4, ".txt") == 0) {
                // Remove .txt extension
                std::string displayName = filename.substr(0, filename.size() - 4);
                // Convert underscores to spaces and capitalize
                for (size_t i = 0; i < displayName.length(); i++) {
                    if (displayName[i] == '_') {
                        displayName[i] = ' ';
                    }
                    if (i == 0 || displayName[i-1] == ' ') {
                        displayName[i] = std::toupper(displayName[i]);
                    }
                }
                templates.push_back(displayName);
            }
        }
    } catch (const fs::filesystem_error&) {
        // If there's an error reading templates, just return what we have
    }
    return templates;
}
std::vector<std::string> getAllFilesRecursive(const std::string& basePath) {
    std::vector<std::string> allFiles;
    if (!fs::exists(basePath) || !fs::is_directory(basePath)) {
        return allFiles;
    }
    try {
        for (const auto& entry : fs::recursive_directory_iterator(basePath, 
            fs::directory_options::skip_permission_denied)) {
            const std::string filename = entry.path().filename().string();
            // Skip hidden lock file
            if (filename == ".jot_locks") {
                continue;
            }
            // Get relative path from base
            const std::string relativePath = fs::relative(entry.path(), basePath).string();
            if (entry.is_regular_file()) {
                // Check if filename starts with {S} to indicate smart note
                if (filename.length() >= 3 && filename.compare(0, 3, "{S}") == 0) {
                    // Replace the filename part with smart note indicator
                    size_t lastSlash = relativePath.find_last_of('/');
                    if (lastSlash != std::string::npos) {
                        allFiles.push_back(relativePath.substr(0, lastSlash + 1) + TEXT_SMART_NOTE_LABEL + filename.substr(3));
                    } else {
                        allFiles.push_back(TEXT_SMART_NOTE_LABEL + filename.substr(3));
                    }
                } else {
                    allFiles.push_back(relativePath);
                }
            } else if (entry.is_directory()) {
                allFiles.push_back(TEXT_FOLDER_LABEL + relativePath);
            }
        }
    } catch (const fs::filesystem_error&) {
        // Handle permission errors or other filesystem issues gracefully
    }
    std::sort(allFiles.begin(), allFiles.end());
    return allFiles;
}
std::string getFullPath(const std::string& filename) {
    std::string actualName = filename;
    if (filename.length() >= 2 && filename.substr(0, 2) == "[]") {
        actualName = filename.substr(2);
    }
    return currentPath + "/" + actualName;
}
std::string getActualName(const std::string& displayName) {
    // Check for folder prefix first (most common case)
    if (displayName.length() >= 2 && displayName.compare(0, 2, TEXT_FOLDER_LABEL) == 0) {
        return displayName.substr(2);
    }
    // Check for smart note prefix
    if (displayName.length() >= 3 && displayName.compare(0, 3, TEXT_SMART_NOTE_LABEL) == 0) {
        return "{S}" + displayName.substr(3);
    }
    // Return as-is if no prefix
    return displayName;
}
std::string getActualFilename(const std::string& displayName) {
    // Check for folder prefix first (most common case)
    if (displayName.length() >= 2 && displayName.compare(0, 2, TEXT_FOLDER_LABEL) == 0) {
        return displayName.substr(2);
    }
    // Check for smart note prefix
    if (displayName.length() >= 3 && displayName.compare(0, 3, TEXT_SMART_NOTE_LABEL) == 0) {
        return "{S}" + displayName.substr(3);
    }
    // Return as-is if no prefix
    return displayName;
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
    if (filename.length() >= 2 && filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        actualName = filename.substr(2);
    }
    if (filename.length() >= 3 && filename.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        actualName = "{S}" + filename.substr(3);
    }
    std::string path = currentPath + "/" + actualName;
    fs::remove_all(path);
}
void renameFile(const std::string& oldName, const std::string& newName) {
    std::string actualOldName = oldName;
    std::string actualNewName = newName;
    bool isFolder = false;
    bool isSmartNote = false;

    // Check if it's a folder
    if (oldName.length() >= 2 && oldName.substr(0, 2) == TEXT_FOLDER_LABEL) {
        actualOldName = oldName.substr(2);
        isFolder = true;
    }

    // Check if it's a smart note
    if (oldName.length() >= 3 && oldName.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        actualOldName = "{S}" + oldName.substr(3);
        isSmartNote = true;
        // Preserve {S} prefix in new name
        if (newName.length() < 3 || newName.substr(0, 3) != "{S}") {
            actualNewName = "{S}" + newName;
        }
    }

    // Check if old file had .txt extension and new name doesn't
    if (!isFolder) {
        size_t dotPos = actualOldName.rfind('.');
        if (dotPos != std::string::npos) {
            std::string oldExt = actualOldName.substr(dotPos);
            // If old file had .txt extension and new name doesn't have any extension, preserve it
            size_t newDotPos = actualNewName.rfind('.');
            if (oldExt == ".txt" && (newDotPos == std::string::npos || newDotPos < actualNewName.length() - 5)) {
                // Check if new name already has .txt
                if (actualNewName.length() < 4 || actualNewName.substr(actualNewName.length() - 4) != ".txt") {
                    actualNewName += ".txt";
                }
            }
        }
    }

    std::string oldPath = currentPath + "/" + actualOldName;
    std::string newPath = currentPath + "/" + actualNewName;
    fs::rename(oldPath, newPath);
}
std::string getTimeAgo(const std::string& filepath) {
    if (!fs::exists(filepath)) {
        return "";
    }
    auto ftime = fs::last_write_time(filepath);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - sctp).count();
    if (diff < 60) {
        return "just now";
    } else if (diff < 3600) {
        int mins = diff / 60;
        return std::to_string(mins) + (mins == 1 ? " minute ago" : " minutes ago");
    } else if (diff < 86400) {
        int hours = diff / 3600;
        return std::to_string(hours) + (hours == 1 ? " hour ago" : " hours ago");
    } else {
        int days = diff / 86400;
        return std::to_string(days) + (days == 1 ? " day ago" : " days ago");
    }
}
void createFileWithTemplate(const std::string& filename, const std::string& templateName) {
    // Check if this is a smart template
    bool isSmartTemplate = (templateName == "Smart");
    // Prepend {S} to the filename if creating a smart note
    std::string actualFilename = filename;
    if (isSmartTemplate) {
        actualFilename = "{S}" + filename;
    }
    std::string filepath = currentPath + "/" + actualFilename;
    // Check if file already exists
    if (fs::exists(filepath)) {
        return;
    }
    if (templateName == TEXT_MSG_BLANK_LABEL) {
        // Create empty file
        std::ofstream outFile(filepath);
        outFile.close();
    } else {
        // Find and copy template content
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
                std::ofstream outFile(filepath);
                outFile << inFile.rdbuf();
                inFile.close();
                outFile.close();
            }
        }
    }
}
void convertMarkdownToDocx(const std::string& filename, std::string& statusMessage) {
    std::string actualFilename = filename;
    // Check if it's a folder
    if (filename.length() >= 2 && filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        statusMessage = TEXT_MSG_CANNOT_CONVERT_DIR;
        return;
    }
    std::string filepath = currentPath + "/" + actualFilename;
    // Check if file is .txt or .md
    size_t dotPos = actualFilename.rfind('.');
    if (dotPos == std::string::npos) {
        statusMessage = TEXT_MSG_FILE_MUST_BE_MD_TXT;
        return;
    }
    std::string ext = actualFilename.substr(dotPos);
    if (ext != ".md" && ext != ".txt") {
        statusMessage = TEXT_MSG_FILE_MUST_BE_MD_TXT;
        return;
    }
    // Create output filename
    std::string baseName = actualFilename.substr(0, dotPos);
    std::string outputFilename = baseName + ".docx";
    std::string outputPath = currentPath + "/" + outputFilename;
    // Use python script to convert
    std::string scriptPath = TEMPLATES_DIR + "/convert_to_mla.py";
    std::string command = "python3 \"" + scriptPath + "\" \"" + filepath + "\" \"" + outputPath + "\" 2>&1";
    // Execute the conversion
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        statusMessage = TEXT_MSG_CONVERSION_FAILED;
        return;
    }
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    int exitCode = pclose(pipe);
    if (exitCode == 0 && fs::exists(outputPath)) {
        statusMessage = std::string(TEXT_MSG_CONVERTED_TO) + outputFilename + TEXT_MSG_MLA_APPLIED;
    } else {
        statusMessage = TEXT_MSG_CONVERSION_FAILED;
    }
}
void openFileInEditor(const std::string& filename) {
    // Don't open folders
    if (filename.length() >= 2 && filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        return;
    }
    // Check if this is a smart note
    bool isSmartNote = (filename.length() >= 3 && filename.substr(0, 3) == TEXT_SMART_NOTE_LABEL);
    // Get actual filename
    std::string actualFilename = filename;
    if (isSmartNote) {
        actualFilename = filename.substr(3); // Remove {S} prefix
        actualFilename = "{S}" + actualFilename; // Add {S} back to actual file name
    }
    std::string filepath = currentPath + "/" + actualFilename;
    def_prog_mode();
    endwin();
    // Open file in micro editor
    std::string command = "micro \"" + filepath + "\"";
    system(command.c_str());
    // Apply smart formatting if this is a smart note
    if (isSmartNote) {
        std::string scriptPath = TEMPLATES_DIR + "/smart_format.py";
        std::string tempPath = filepath + ".tmp";
        // Log formatting attempt
        system("echo '--- Format attempt ---' >> /tmp/jot_format.log");
        std::string formatCmd = "python3 \"" + scriptPath + "\" \"" + filepath + "\" \"" + tempPath + "\" 2>&1 >> /tmp/jot_format.log";
        system(("echo 'Command: " + formatCmd + "' >> /tmp/jot_format.log").c_str());
        int result = system(formatCmd.c_str());
        system(("echo 'Exit code: " + std::to_string(result / 256) + "' >> /tmp/jot_format.log").c_str());
        // If formatting succeeded and temp file was created, replace original
        if (result == 0 && fs::exists(tempPath)) {
            fs::rename(tempPath, filepath);
            system("echo 'File renamed successfully' >> /tmp/jot_format.log");
        } else {
            system("echo 'Format failed or temp file not created' >> /tmp/jot_format.log");
        }
    }
    // Sync to cloud if enabled
    if (g_cloudSyncEnabled && !g_currentRemote.empty()) {
        syncToCloud(true); // Silent sync after file edit
    }
    refresh();
}
