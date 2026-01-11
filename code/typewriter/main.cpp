#include <ncurses.h>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <set>

namespace fs = std::filesystem;

// Directory configuration
// Cloud storage options:
// 1. Local only: "files"
// 2. Rclone mount: "/home/pi/cloud/files" (or your mounted path)
// 3. Synced folder: point to your rclone sync directory
const std::string BASE_DIR = "files";
const std::string TEMPLATES_DIR = "templates";
std::string currentPath = BASE_DIR;

// Cloud sync configuration (rclone)
// To find your remote name, run: rclone listremotes
// Your configured remote: "Google Drive" ✅
const bool ENABLE_CLOUD_SYNC = true; // Cloud sync is DISABLED
const std::string RCLONE_REMOTE = "Google Drive"; // Your Google Drive remote
const std::string RCLONE_PATH = "typewriter/files"; // Path on your cloud storage
const int SYNC_DELAY_SECONDS = 2; // Delay after file operations before syncing

// Runtime settings (loaded from config file, can be changed in settings menu)
std::string g_currentRemote = RCLONE_REMOTE;
bool g_cloudSyncEnabled = ENABLE_CLOUD_SYNC;
bool g_autoSyncEnabled = true;

// Sync type enum
enum SyncType {
    SYNC_TYPE_FULL = 0,      // Bidirectional - mirrors changes both ways
    SYNC_TYPE_ONE_WAY = 1    // One-way - only uploads, never deletes from cloud
};
SyncType g_syncType = SYNC_TYPE_ONE_WAY; // Default to safer one-way sync


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
const char KEY_SELECT_MODE = 's'; // Selection mode toggle
const char KEY_MOVE_ITEMS = 'v'; // Move selected items
const char KEY_UNDO_ACTION = 'z'; // Undo last action
const char KEY_SETTINGS = 't'; // Settings menu
const char KEY_QUIT = 'q';

// UI Text - easy to customize
const char* TEXT_HEADER = "Happy Writing";
const char* TEXT_SEPARATOR = "--------------------------------------";
const char* TEXT_HOME_FOLDER = "Presenting the home";
const char* TEXT_YOU_ARE_IN = "Presenting %s";
const char* TEXT_PAGE_INDICATOR = "Page %d of %d";
const char* TEXT_NO_FILES = "empty canvas";
const char* TEXT_CONTROLS = "Controls:";
const char* TEXT_FOLDER_LABEL = "[]";
const char* TEXT_SMART_NOTE_LABEL = "{S}";
const char* TEXT_STATUS_PREFIX = "Status: ";

// Control labels
const char* TEXT_ARROWS = "arrows: Navigate";
const char* TEXT_ENTER = "enter: Open";
const char* TEXT_NEW_FILE_LABEL = "New note";
const char* TEXT_NEW_FOLDER_LABEL = "New Folder";
const char* TEXT_RENAME_LABEL = "Retitle";
const char* TEXT_DELETE_LABEL = "Scrap";
const char* TEXT_DELETE_KEY_LABEL = "DEL"; // Display name for delete key
const char* TEXT_CONVERT_LABEL = "Convert";
const char* TEXT_BACK_LABEL = "Back";
const char* TEXT_QUIT_LABEL = "Leave";
const char* TEXT_SELECT_LABEL = "Choose";
const char* TEXT_UNDO_LABEL = "Rewind";
const char* TEXT_SETTINGS_LABEL = "Settings";

// Settings menu text
const char* TEXT_SETTINGS_HEADER = "Cloud Sync Settings";
const char* TEXT_SETTINGS_CURRENT_REMOTE = "Current Remote: ";
const char* TEXT_SETTINGS_CLOUD_SYNC = "Cloud Sync: ";
const char* TEXT_SETTINGS_AUTO_SYNC = "Auto Sync: ";
const char* TEXT_SETTINGS_ENABLED = "enabled";
const char* TEXT_SETTINGS_DISABLED = "disabled";
const char* TEXT_SETTINGS_ON = "on";
const char* TEXT_SETTINGS_OFF = "of (manual)";
const char* TEXT_SETTINGS_NONE = "(none)";
const char* TEXT_SETTINGS_OPTIONS = "options:";
const char* TEXT_SETTINGS_NAV = "Use arrow keys to navigate, enter to select, escape to go withdraw";
const char* TEXT_SETTINGS_OPT_VIEW = "1. View Remotes";
const char* TEXT_SETTINGS_OPT_STATUS = "2. Sync Status";
const char* TEXT_SETTINGS_OPT_SWITCH = "3. Switch Remote";
const char* TEXT_SETTINGS_OPT_TOGGLE_CLOUD = "4. Toggle Cloud Sync (Enable/Disable)";
const char* TEXT_SETTINGS_OPT_TOGGLE_AUTO = "5. Toggle Auto Sync Mode";
const char* TEXT_SETTINGS_OPT_SYNC_NOW = "6. Sync Now (Manual)";
const char* TEXT_SETTINGS_OPT_SETUP = "7. Setup New Remote";
const char* TEXT_SETTINGS_OPT_BACK = "8. Back to Files";
const char* TEXT_SETTINGS_SELECT_REMOTE = "Select Remote";
const char* TEXT_SETTINGS_ENTER_SELECT = "ENTER to select, ESC to cancel";
const char* TEXT_SETTINGS_NO_REMOTES = "No remotes configured. Setup a new remote first.";
const char* TEXT_SETTINGS_SYNCING = "Syncing with cloud...";
const char* TEXT_SETTINGS_SYNC_COMPLETE = "Sync complete!";
const char* TEXT_SETTINGS_SYNC_DISABLED = "Cloud sync is disabled or no remote selected.";
const char* TEXT_SETTINGS_SETUP_HEADER = "=== Setup New rclone Remote ===";
const char* TEXT_SETTINGS_SETUP_RUNNING = "Running: rclone config";
const char* TEXT_SETTINGS_SETUP_RETURN = "\nPress ENTER to return to settings...";
const char* TEXT_SETTINGS_VIEW_HEADER = "\n=== Configured Remotes ===";
const char* TEXT_SETTINGS_VIEW_CONTINUE = "\nPress ENTER to continue...";
const char* TEXT_SETTINGS_STATUS_CHECKING = "Checking sync status...";
const char* TEXT_SETTINGS_STATUS_CLOUD_DISABLED = "Cloud sync is currently DISABLED";
const char* TEXT_SETTINGS_STATUS_MODE_AUTO = "Mode: AUTOMATIC (syncs after each file save)";
const char* TEXT_SETTINGS_STATUS_MODE_MANUAL = "Mode: MANUAL (sync when you choose)";
const char* TEXT_SETTINGS_STATUS_CONNECTED = "Connected to: ";
const char* TEXT_SETTINGS_STATUS_PATH = "Cloud path: ";
const char* TEXT_SETTINGS_STATUS_LOCAL = "Local path: ";
const char* TEXT_SETTINGS_CONFIG_FILE = "typewriter_config.txt";
const char* TEXT_SETTINGS_TESTING_CONNECTION = "Testing connection...";
const char* TEXT_SETTINGS_CONNECTION_OK = "✓ Connection successful!";
const char* TEXT_SETTINGS_CONNECTION_FAILED = "✗ Connection failed - Remote needs reconfiguration";
const char* TEXT_SETTINGS_CONNECTION_HELP = "Run 'rclone config' to fix authentication issues";
const char* TEXT_SETTINGS_FIX_REMOTE = "9. Fix/Reconnect Remote";
const char* TEXT_SETTINGS_OPT_TEST_CONNECTION = "10. Test Connection";
const char* TEXT_SETTINGS_OPT_SYNC_TYPE = "11. Change Sync Type";
const char* TEXT_SETTINGS_OPT_BACK_NEW = "12. Back to Files";

// Sync type constants
const char* TEXT_SYNC_TYPE_FULL = "FULL SYNC (bidirectional)";
const char* TEXT_SYNC_TYPE_ONE_WAY = "ONE-WAY (upload only)";
const char* TEXT_SETTINGS_SYNC_TYPE = "Sync Type: ";

// Connection test messages
const char* TEXT_CONNECTION_TEST_HEADER = "Connection Test";
const char* TEXT_CONNECTION_TEST_TESTING = "Testing connection to: ";
const char* TEXT_CONNECTION_TEST_SUCCESS = "✓ SUCCESS! Connection is working perfectly.";
const char* TEXT_CONNECTION_TEST_FAILED = "✗ FAILED! Cannot connect to remote.";
const char* TEXT_CONNECTION_TEST_NO_REMOTE = "No remote selected. Please select a remote first (Option 3).";
const char* TEXT_CONNECTION_TEST_SYNC_DISABLED = "Cloud sync is disabled. Enable it first (Option 4).";
const char* TEXT_CONNECTION_FIX_PROMPT = "Would you like to fix this connection? (y/n): ";
const char* TEXT_CONNECTION_PRESS_KEY = "Press any key to continue...";

// Interactive fix wizard messages
const char* TEXT_FIX_WIZARD_HEADER = "Connection Fix Wizard";
const char* TEXT_FIX_WIZARD_PROBLEM = "Problem detected: Cannot authenticate with remote";
const char* TEXT_FIX_WIZARD_SOLUTION = "Recommended Solution:";
const char* TEXT_FIX_WIZARD_OPTION_1 = "1. Quick Reconnect (Recommended)";
const char* TEXT_FIX_WIZARD_OPTION_2 = "2. Full Reconfiguration";
const char* TEXT_FIX_WIZARD_OPTION_3 = "3. Cancel";
const char* TEXT_FIX_WIZARD_RECONNECT_INFO = "   This will refresh your authentication token.";
const char* TEXT_FIX_WIZARD_RECONFIG_INFO = "   This will guide you through a complete setup.";
const char* TEXT_FIX_WIZARD_RUNNING = "Running authentication...";
const char* TEXT_FIX_WIZARD_SUCCESS = "✓ Connection fixed successfully!";
const char* TEXT_FIX_WIZARD_FAILED = "✗ Fix failed. You may need to delete and recreate the remote.";
const char* TEXT_FIX_WIZARD_INSTRUCTIONS = "Follow the prompts in the terminal to complete authentication.";
const char* TEXT_FIX_WIZARD_BROWSER = "Your browser will open for authentication.";
const char* TEXT_FIX_WIZARD_TESTING = "Testing connection again...";

// Sync type selection messages
const char* TEXT_SYNC_TYPE_HEADER = "Select Sync Type";
const char* TEXT_SYNC_TYPE_FULL_DESC = "1. Full Sync (Bidirectional)";
const char* TEXT_SYNC_TYPE_FULL_DETAIL = "   Files are synced both ways. Changes on cloud or local are mirrored.";
const char* TEXT_SYNC_TYPE_FULL_WARNING = "   WARNING: Deleting files locally will delete them on cloud!";
const char* TEXT_SYNC_TYPE_ONE_WAY_DESC = "2. One-Way Sync (Upload Only)";
const char* TEXT_SYNC_TYPE_ONE_WAY_DETAIL = "   Files are only uploaded to cloud. Deletions are local only.";
const char* TEXT_SYNC_TYPE_ONE_WAY_SAFE = "   SAFE: Deleted files remain on cloud as backup.";
const char* TEXT_SYNC_TYPE_CURRENT = "Current: ";
const char* TEXT_SYNC_TYPE_SELECT = "Select sync type (1 or 2), or ESC to cancel: ";

// Prompt messages
const char* TEXT_PROMPT_NEW_FILE = "Enter the note's title (X - esc): ";
const char* TEXT_PROMPT_NEW_FOLDER = "Enter the folder's title (X - esc): ";
const char* TEXT_PROMPT_RENAME = "Enter new name for '%s' (X - esc): ";
const char* TEXT_PROMPT_DELETE_CONFIRM = "scrap %s? (y/n): ";
const char* TEXT_PROMPT_SELECT_TEMPLATE = "Select a Canvas";
const char* TEXT_PROMPT_TEMPLATE_NAV = "Use arrow keys to navigate, ENTER to select, ESC to cancel";

// Status messages
const char* TEXT_MSG_IDLE = "Available";
const char* TEXT_MSG_OPENED_FOLDER = "Entered folder: ";
const char* TEXT_MSG_EDITED_FILE = "Revised ";
const char* TEXT_MSG_RETURNED = "Returned to parent folder";
const char* TEXT_MSG_CREATED_FILE = "New note ";
const char* TEXT_MSG_CREATED_FOLDER = "New folder ";
const char* TEXT_MSG_USER_CANCELLED = "Withdrew choice";
const char* TEXT_MSG_ALREADY_EXISTS = "name already exists.";
const char* TEXT_MSG_RENAMED = "Retitled to ";
const char* TEXT_MSG_RENAME_CANCELLED = "Kept title";
const char* TEXT_MSG_NO_FILES_RENAME = "No notes to retitle.";
const char* TEXT_MSG_DELETED = "Scrapped ";
const char* TEXT_MSG_DELETE_CANCELLED = "Kept file";
const char* TEXT_MSG_NO_FILES_DELETE = "No files to scrap.";

// Selection messages
const char* TEXT_MSG_ITEMS_SELECTED = " item(s) chosen ('esc' to deselect, 'v' to relocate)";
const char* TEXT_MSG_SELECTION_MODE_ON = "Selection mode ON (press 's' again to lock, use arrows to select)";
const char* TEXT_MSG_SELECTION_LOCKED = "Selection locked (";
const char* TEXT_MSG_SELECTION_CANCELLED = "Selection cancelled";
const char* TEXT_MSG_NO_ITEMS_TO_MOVE = "No assets chosen to move";

// Move operation messages
const char* TEXT_MSG_MOVED_ITEMS = "Relocated ";
const char* TEXT_MSG_MOVE_FAILED = " failed)";
const char* TEXT_MSG_MOVE_ERROR = "Relocation failed";
const char* TEXT_MSG_NO_ITEMS_MOVE = "No assets to move";

// File operation messages
const char* TEXT_MSG_OPENED = "Entered ";
const char* TEXT_MSG_CANNOT_CONVERT_DIR = "Cannot convert a directory";
const char* TEXT_MSG_FILE_MUST_BE_MD_TXT = "note must be .md or .txt format";
const char* TEXT_MSG_CONVERTING = "Converting to Word document with MLA formatting...";
const char* TEXT_MSG_CONVERTED_TO = "Converted to ";
const char* TEXT_MSG_MLA_APPLIED = " (MLA formatting applied)";
const char* TEXT_MSG_CONVERSION_FAILED = "Conversion failed"; // (- check /tmp/mla_convert.log, ensure python-docx is installed)
const char* TEXT_MSG_NO_FILE_FOR_CONVERSION = "No file chosen for conversion";
const char* TEXT_MSG_BLANK_LABEL = "empty";

// Undo messages
const char* TEXT_MSG_UNDONE = "Rewound ";
const char* TEXT_MSG_NOTHING_TO_UNDO = "No actions to rewind";
const char* TEXT_MSG_UNDO_FAILED = "Rewind failed";

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
                std::string filename = entry.path().filename().string();
                // Check if filename starts with {S} to indicate smart note
                if (filename.length() >= 3 && filename.substr(0, 3) == "{S}") {
                    // Already has {S} prefix, add it to display
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

// Function to get available templates
std::vector<std::string> getTemplates() {
    std::vector<std::string> templates;
    templates.push_back(TEXT_MSG_BLANK_LABEL); // Always include blank option first

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

// Global selection state (full paths to selected items)
std::set<std::string> g_selectedPaths; // All selected items
bool g_selectMode = false; // true when in selection mode (press 's' to toggle)
int g_anchorIndex = -1; // The index where selection mode started
std::string g_anchorPath = ""; // The item where selection mode started

// Undo system
enum UndoActionType {
    UNDO_NONE,
    UNDO_CREATE_FILE,
    UNDO_CREATE_FOLDER,
    UNDO_DELETE,
    UNDO_RENAME,
    UNDO_MOVE
};

struct UndoAction {
    UndoActionType type;
    std::string path1; // source path or created item path
    std::string path2; // destination path for rename/move
    std::string content; // file content for deleted files
    bool wasDirectory; // whether deleted item was a directory
    std::vector<std::string> movedPaths; // for multiple file moves
    std::vector<std::string> destPaths; // destination paths for moves
};

UndoAction g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};

// Helper: Get full path for a file/folder name in current directory
std::string getFullPath(const std::string& filename) {
    std::string actualName = filename;
    if (filename.substr(0, 2) == "[]") {
        actualName = filename.substr(2);
    }
    return currentPath + "/" + actualName;
}

// Helper: Get actual filename without folder prefix
std::string getActualName(const std::string& displayName) {
    if (displayName.substr(0, 2) == "[]") {
        return displayName.substr(2);
    }
    return displayName;
}

// Helper: Check if a path is currently selected
bool isPathSelected(const std::string& fullPath) {
    return g_selectedPaths.find(fullPath) != g_selectedPaths.end();
}

// Helper: Toggle selection for a path
void toggleSelection(const std::string& fullPath) {
    if (isPathSelected(fullPath)) {
        g_selectedPaths.erase(fullPath);
    } else {
        g_selectedPaths.insert(fullPath);
    }
}

// Function to perform undo operation
bool performUndo(std::string& statusMessage) {
    if (g_lastAction.type == UNDO_NONE) {
        statusMessage = TEXT_MSG_NOTHING_TO_UNDO;
        return false;
    }

    try {
        switch (g_lastAction.type) {
            case UNDO_CREATE_FILE:
            case UNDO_CREATE_FOLDER: {
                // Undo creation by deleting the created item
                if (fs::exists(g_lastAction.path1)) {
                    fs::remove_all(g_lastAction.path1);
                    statusMessage = std::string(TEXT_MSG_UNDONE) + "created " + fs::path(g_lastAction.path1).filename().string();
                    g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                    return true;
                }
                break;
            }

            case UNDO_DELETE: {
                // Undo deletion by restoring the item
                if (g_lastAction.wasDirectory) {
                    // Restore directory
                    fs::create_directory(g_lastAction.path1);
                } else {
                    // Restore file with its content
                    std::ofstream outFile(g_lastAction.path1);
                    if (outFile.is_open()) {
                        outFile << g_lastAction.content;
                        outFile.close();
                    }
                }
                statusMessage = std::string(TEXT_MSG_UNDONE) + "deleted " + fs::path(g_lastAction.path1).filename().string();
                g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                return true;
            }

            case UNDO_RENAME: {
                // Undo rename by renaming back
                if (fs::exists(g_lastAction.path2)) {
                    fs::rename(g_lastAction.path2, g_lastAction.path1);
                    statusMessage = std::string(TEXT_MSG_UNDONE) + "renamed " + fs::path(g_lastAction.path2).filename().string();
                    g_lastAction = {UNDO_NONE, "", "", "", false, {}, {}};
                    return true;
                }
                break;
            }

            case UNDO_MOVE: {
                // Undo move by moving items back
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

// Cloud sync functions
bool testCloudConnection() {
    if (g_currentRemote.empty()) return false;

    // Test connection by listing remote root
    std::string cmd = "rclone lsd \"" + g_currentRemote + ":\" --max-depth 1 > /dev/null 2>&1";
    int result = system(cmd.c_str());
    return (result == 0);
}

void syncToCloud(bool background = true) {
    if (!g_cloudSyncEnabled || g_currentRemote.empty()) return;

    std::string cmd;

    if (g_syncType == SYNC_TYPE_FULL) {
        // Full bidirectional sync - mirrors both ways (deletes on both sides)
        cmd = "rclone sync \"" + BASE_DIR + "\" \"" + g_currentRemote + ":" + RCLONE_PATH +
              "\" --exclude '.git/**' 2>/dev/null";
    } else {
        // One-way sync - only upload/update, never delete from cloud
        cmd = "rclone copy \"" + BASE_DIR + "\" \"" + g_currentRemote + ":" + RCLONE_PATH +
              "\" --exclude '.git/**' 2>/dev/null";
    }

    if (background) {
        cmd += " &"; // Run in background
    }

    system(cmd.c_str());
}

void syncFromCloud() {
    if (!g_cloudSyncEnabled || g_currentRemote.empty()) return;

    // Download changes from cloud to local
    std::string cmd = "rclone copy \"" + g_currentRemote + ":" + RCLONE_PATH + "\" \"" + BASE_DIR +
                      "\" --exclude '.git/**' 2>/dev/null";

    system(cmd.c_str());
}

void bidirectionalSync() {
    if (!g_cloudSyncEnabled || g_currentRemote.empty()) return;

    // Two-way sync (be careful - this can delete files)
    std::string cmd = "rclone sync \"" + BASE_DIR + "\" \"" + g_currentRemote + ":" + RCLONE_PATH +
                      "\" --exclude '.git/**' 2>/dev/null &";

    system(cmd.c_str());
}

// Settings management functions
void saveSettings() {
    std::ofstream config(TEXT_SETTINGS_CONFIG_FILE);
    if (config.is_open()) {
        config << "remote=" << g_currentRemote << "\n";
        config << "enabled=" << (g_cloudSyncEnabled ? "1" : "0") << "\n";
        config << "auto_sync=" << (g_autoSyncEnabled ? "1" : "0") << "\n";
        config << "sync_type=" << static_cast<int>(g_syncType) << "\n";
        config.close();
    }
}

void loadSettings() {
    std::ifstream config(TEXT_SETTINGS_CONFIG_FILE);
    if (config.is_open()) {
        std::string line;
        while (std::getline(config, line)) {
            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string key = line.substr(0, eq);
                std::string value = line.substr(eq + 1);

                if (key == "remote") g_currentRemote = value;
                else if (key == "enabled") g_cloudSyncEnabled = (value == "1");
                else if (key == "auto_sync") g_autoSyncEnabled = (value == "1");
                else if (key == "sync_type") {
                    int syncTypeInt = std::stoi(value);
                    g_syncType = static_cast<SyncType>(syncTypeInt);
                }
            }
        }
        config.close();
    }
}

std::vector<std::string> listRcloneRemotes() {
    std::vector<std::string> remotes;
    FILE* pipe = popen("rclone listremotes 2>/dev/null", "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            std::string remote = buffer;
            // Remove trailing newline and colon
            if (!remote.empty() && remote.back() == '\n') {
                remote.pop_back();
            }
            if (!remote.empty() && remote.back() == ':') {
                remote.pop_back();
            }
            if (!remote.empty()) {
                remotes.push_back(remote);
            }
        }
        pclose(pipe);
    }
    return remotes;
}

// Forward declaration
void drawSeparator(int y);

// Interactive connection test with fix option
void showConnectionTest() {
    clear();
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_CONNECTION_TEST_HEADER);
    attroff(A_BOLD);
    drawSeparator(1);

    // Check if sync is enabled and remote is set
    if (!g_cloudSyncEnabled) {
        mvprintw(3, 2, TEXT_CONNECTION_TEST_SYNC_DISABLED);
        mvprintw(LINES - 2, 0, TEXT_CONNECTION_PRESS_KEY);
        refresh();
        getch();
        return;
    }

    if (g_currentRemote.empty()) {
        mvprintw(3, 2, TEXT_CONNECTION_TEST_NO_REMOTE);
        mvprintw(LINES - 2, 0, TEXT_CONNECTION_PRESS_KEY);
        refresh();
        getch();
        return;
    }

    // Test connection
    mvprintw(3, 2, "%s%s", TEXT_CONNECTION_TEST_TESTING, g_currentRemote.c_str());
    refresh();
    sleep(1);

    bool connected = testCloudConnection();

    clear();
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_CONNECTION_TEST_HEADER);
    attroff(A_BOLD);
    drawSeparator(1);

    if (connected) {
        attron(A_BOLD);
        mvprintw(3, 2, TEXT_CONNECTION_TEST_SUCCESS);
        attroff(A_BOLD);
        mvprintw(5, 2, "Remote: %s", g_currentRemote.c_str());
        mvprintw(6, 2, "Status: Connected and ready");
        mvprintw(LINES - 2, 0, TEXT_CONNECTION_PRESS_KEY);
    } else {
        attron(A_BOLD);
        mvprintw(3, 2, TEXT_CONNECTION_TEST_FAILED);
        attroff(A_BOLD);
        mvprintw(5, 2, "Remote: %s", g_currentRemote.c_str());
        mvprintw(6, 2, "Status: Authentication error or network issue");
        mvprintw(8, 2, TEXT_CONNECTION_FIX_PROMPT);
    }

    refresh();

    if (!connected) {
        // Offer to fix
        int ch = getch();
        if (ch == 'y' || ch == 'Y' || ch == '\n' || ch == KEY_ENTER) {
            // Show fix wizard
            clear();
            attron(A_BOLD);
            mvprintw(0, 0, TEXT_FIX_WIZARD_HEADER);
            attroff(A_BOLD);
            drawSeparator(1);

            mvprintw(3, 2, TEXT_FIX_WIZARD_PROBLEM);
            mvprintw(4, 2, "Remote: %s", g_currentRemote.c_str());

            drawSeparator(6);
            mvprintw(7, 2, TEXT_FIX_WIZARD_SOLUTION);
            mvprintw(9, 2, TEXT_FIX_WIZARD_OPTION_1);
            mvprintw(10, 2, TEXT_FIX_WIZARD_RECONNECT_INFO);
            mvprintw(12, 2, TEXT_FIX_WIZARD_OPTION_2);
            mvprintw(13, 2, TEXT_FIX_WIZARD_RECONFIG_INFO);
            mvprintw(15, 2, TEXT_FIX_WIZARD_OPTION_3);

            drawSeparator(17);
            mvprintw(18, 0, "Select option (1, 2, or 3): ");
            refresh();

            int option = getch();

            if (option == '1') {
                // Quick reconnect
                def_prog_mode();
                endwin();

                std::cout << "\n" << TEXT_FIX_WIZARD_HEADER << "\n";
                std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
                std::cout << "Reconnecting to: " << g_currentRemote << "\n\n";
                std::cout << TEXT_FIX_WIZARD_INSTRUCTIONS << "\n";
                std::cout << TEXT_FIX_WIZARD_BROWSER << "\n\n";

                std::string cmd = "rclone config reconnect \"" + g_currentRemote + "\"";
                int result = system(cmd.c_str());

                if (result != 0) {
                    std::cout << "\nReconnect not available, trying full config...\n";
                    std::cout << "In the menu, select your remote and choose 'reconnect'\n\n";
                    system("rclone config");
                }

                std::cout << "\n" << TEXT_FIX_WIZARD_TESTING << "\n";
                sleep(1);

                reset_prog_mode();
                refresh();

                // Test again
                if (testCloudConnection()) {
                    clear();
                    attron(A_BOLD);
                    mvprintw(LINES/2, (COLS - 40)/2, TEXT_FIX_WIZARD_SUCCESS);
                    attroff(A_BOLD);
                    mvprintw(LINES/2 + 2, (COLS - 40)/2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                } else {
                    clear();
                    mvprintw(LINES/2, 2, TEXT_FIX_WIZARD_FAILED);
                    mvprintw(LINES/2 + 2, 2, "You may need to delete and recreate the remote.");
                    mvprintw(LINES/2 + 3, 2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                }
            } else if (option == '2') {
                // Full reconfiguration
                def_prog_mode();
                endwin();

                std::cout << "\n" << TEXT_FIX_WIZARD_HEADER << "\n";
                std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
                std::cout << "Opening rclone configuration...\n\n";
                std::cout << "Steps:\n";
                std::cout << "1. Select your remote: " << g_currentRemote << "\n";
                std::cout << "2. Choose 'Edit existing remote' or 'reconnect'\n";
                std::cout << "3. Follow the authentication prompts\n";
                std::cout << "4. " << TEXT_FIX_WIZARD_BROWSER << "\n\n";
                std::cout << "Press ENTER to continue...";
                std::cin.get();

                system("rclone config");

                std::cout << "\n" << TEXT_FIX_WIZARD_TESTING << "\n";
                sleep(1);

                reset_prog_mode();
                refresh();

                // Test again
                if (testCloudConnection()) {
                    clear();
                    attron(A_BOLD);
                    mvprintw(LINES/2, (COLS - 40)/2, TEXT_FIX_WIZARD_SUCCESS);
                    attroff(A_BOLD);
                    mvprintw(LINES/2 + 2, (COLS - 40)/2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                } else {
                    clear();
                    mvprintw(LINES/2, 2, TEXT_FIX_WIZARD_FAILED);
                    mvprintw(LINES/2 + 2, 2, TEXT_CONNECTION_PRESS_KEY);
                    refresh();
                    getch();
                }
            }
            // Option 3 or any other key: cancel
        }
    } else {
        getch();
    }
}

// Sync type selection
void showSyncTypeMenu() {
    clear();
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_SYNC_TYPE_HEADER);
    attroff(A_BOLD);
    drawSeparator(1);

    const char* currentType = (g_syncType == SYNC_TYPE_FULL) ? TEXT_SYNC_TYPE_FULL : TEXT_SYNC_TYPE_ONE_WAY;
    mvprintw(3, 2, "%s%s", TEXT_SYNC_TYPE_CURRENT, currentType);

    drawSeparator(5);

    attron(A_BOLD);
    mvprintw(7, 2, TEXT_SYNC_TYPE_FULL_DESC);
    attroff(A_BOLD);
    mvprintw(8, 2, TEXT_SYNC_TYPE_FULL_DETAIL);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(9, 2, TEXT_SYNC_TYPE_FULL_WARNING);
    attroff(COLOR_PAIR(1) | A_BOLD);

    mvprintw(11, 2, "");
    attron(A_BOLD);
    mvprintw(11, 2, TEXT_SYNC_TYPE_ONE_WAY_DESC);
    attroff(A_BOLD);
    mvprintw(12, 2, TEXT_SYNC_TYPE_ONE_WAY_DETAIL);
    attron(A_BOLD);
    mvprintw(13, 2, TEXT_SYNC_TYPE_ONE_WAY_SAFE);
    attroff(A_BOLD);

    drawSeparator(15);
    mvprintw(16, 0, TEXT_SYNC_TYPE_SELECT);
    refresh();

    int ch = getch();
    if (ch == '1') {
        g_syncType = SYNC_TYPE_FULL;
        saveSettings();
    } else if (ch == '2') {
        g_syncType = SYNC_TYPE_ONE_WAY;
        saveSettings();
    }
}

// Settings menu UI
void showSettingsMenu() {
    int selectedOption = 0;
    bool inMenu = true;

    while (inMenu) {
        clear();

        // Header
        attron(A_BOLD);
        mvprintw(0, 0, TEXT_SETTINGS_HEADER);
        attroff(A_BOLD);
        drawSeparator(1);

        // Current settings display
        mvprintw(3, 2, "%s%s", TEXT_SETTINGS_CURRENT_REMOTE,
                 g_currentRemote.empty() ? TEXT_SETTINGS_NONE : g_currentRemote.c_str());
        mvprintw(4, 2, "%s%s", TEXT_SETTINGS_CLOUD_SYNC,
                 g_cloudSyncEnabled ? TEXT_SETTINGS_ENABLED : TEXT_SETTINGS_DISABLED);
        mvprintw(5, 2, "%s%s", TEXT_SETTINGS_AUTO_SYNC,
                 g_autoSyncEnabled ? TEXT_SETTINGS_ON : TEXT_SETTINGS_OFF);
        const char* syncTypeStr = (g_syncType == SYNC_TYPE_FULL) ? TEXT_SYNC_TYPE_FULL : TEXT_SYNC_TYPE_ONE_WAY;
        mvprintw(6, 2, "%s%s", TEXT_SETTINGS_SYNC_TYPE, syncTypeStr);

        drawSeparator(8);
        mvprintw(9, 0, TEXT_SETTINGS_OPTIONS);

        // Menu options
        const char* options[] = {
            TEXT_SETTINGS_OPT_VIEW,
            TEXT_SETTINGS_OPT_STATUS,
            TEXT_SETTINGS_OPT_SWITCH,
            TEXT_SETTINGS_OPT_TOGGLE_CLOUD,
            TEXT_SETTINGS_OPT_TOGGLE_AUTO,
            TEXT_SETTINGS_OPT_SYNC_NOW,
            TEXT_SETTINGS_OPT_SETUP,
            TEXT_SETTINGS_FIX_REMOTE,
            TEXT_SETTINGS_OPT_TEST_CONNECTION,
            TEXT_SETTINGS_OPT_SYNC_TYPE,
            TEXT_SETTINGS_OPT_BACK_NEW
        };

        for (int i = 0; i < 11; i++) {
            if (i == selectedOption) {
                attron(A_REVERSE);
            }
            mvprintw(11 + i, 2, "%s", options[i]);
            if (i == selectedOption) {
                attroff(A_REVERSE);
            }
        }

        drawSeparator(23);
        mvprintw(24, 0, TEXT_SETTINGS_NAV);

        refresh();

        int ch = getch();
        switch(ch) {
            case KEY_UP:
                if (selectedOption > 0) selectedOption--;
                break;
            case KEY_DOWN:
                if (selectedOption < 10) selectedOption++;
                break;
            case '\n':
            case KEY_ENTER:
                switch(selectedOption) {
                    case 0: { // View Remotes
                        def_prog_mode();
                        endwin();

                        std::cout << TEXT_SETTINGS_VIEW_HEADER << "\n";
                        system("rclone listremotes");
                        std::cout << TEXT_SETTINGS_VIEW_CONTINUE;
                        std::cin.get();

                        reset_prog_mode();
                        refresh();
                        break;
                    }
                    case 1: { // Sync Status
                        clear();
                        mvprintw(0, 0, TEXT_SETTINGS_STATUS_CHECKING);
                        refresh();

                        bool connectionOk = false;
                        if (g_cloudSyncEnabled && !g_currentRemote.empty()) {
                            mvprintw(1, 0, TEXT_SETTINGS_TESTING_CONNECTION);
                            refresh();
                            connectionOk = testCloudConnection();
                        }

                        sleep(1);

                        clear();
                        attron(A_BOLD);
                        mvprintw(0, 0, "Sync Status");
                        attroff(A_BOLD);
                        drawSeparator(1);

                        if (!g_cloudSyncEnabled) {
                            mvprintw(3, 2, TEXT_SETTINGS_STATUS_CLOUD_DISABLED);
                        } else {
                            mvprintw(3, 2, "%s%s", TEXT_SETTINGS_STATUS_CONNECTED, g_currentRemote.c_str());
                            std::string cloudPath = g_currentRemote + ":" + RCLONE_PATH;
                            mvprintw(4, 2, "%s%s", TEXT_SETTINGS_STATUS_PATH, cloudPath.c_str());
                            mvprintw(5, 2, "%s%s", TEXT_SETTINGS_STATUS_LOCAL, BASE_DIR.c_str());
                            mvprintw(7, 2, "%s", g_autoSyncEnabled ? TEXT_SETTINGS_STATUS_MODE_AUTO : TEXT_SETTINGS_STATUS_MODE_MANUAL);

                            // Show connection status
                            mvprintw(9, 2, "Connection test: %s", connectionOk ? TEXT_SETTINGS_CONNECTION_OK : TEXT_SETTINGS_CONNECTION_FAILED);
                            if (!connectionOk) {
                                mvprintw(10, 2, TEXT_SETTINGS_CONNECTION_HELP);
                            }
                        }

                        mvprintw(LINES - 2, 0, "Press any key to continue...");
                        refresh();
                        getch();
                        break;
                    }
                    case 2: { // Switch Remote
                        std::vector<std::string> remotes = listRcloneRemotes();
                        if (remotes.empty()) {
                            mvprintw(LINES - 3, 0, TEXT_SETTINGS_NO_REMOTES);
                            clrtoeol();
                            refresh();
                            sleep(2);
                        } else {
                            int remoteIdx = 0;
                            bool selecting = true;

                            while (selecting) {
                                clear();
                                attron(A_BOLD);
                                mvprintw(0, 0, TEXT_SETTINGS_SELECT_REMOTE);
                                attroff(A_BOLD);
                                drawSeparator(1);

                                for (size_t i = 0; i < remotes.size(); i++) {
                                    if (i == remoteIdx) attron(A_REVERSE);
                                    mvprintw(3 + i, 2, "%s", remotes[i].c_str());
                                    if (i == remoteIdx) attroff(A_REVERSE);
                                }

                                mvprintw(LINES - 2, 0, TEXT_SETTINGS_ENTER_SELECT);
                                refresh();

                                int ch2 = getch();
                                if (ch2 == KEY_UP && remoteIdx > 0) remoteIdx--;
                                else if (ch2 == KEY_DOWN && remoteIdx < (int)remotes.size() - 1) remoteIdx++;
                                else if (ch2 == '\n' || ch2 == KEY_ENTER) {
                                    g_currentRemote = remotes[remoteIdx];
                                    saveSettings();
                                    selecting = false;
                                } else if (ch2 == 27) {
                                    selecting = false;
                                }
                            }
                        }
                        break;
                    }
                    case 3: // Toggle Cloud Sync
                        g_cloudSyncEnabled = !g_cloudSyncEnabled;
                        saveSettings();
                        break;
                    case 4: // Toggle Auto Sync
                        g_autoSyncEnabled = !g_autoSyncEnabled;
                        saveSettings();
                        break;
                    case 5: { // Sync Now
                        mvprintw(LINES - 3, 0, TEXT_SETTINGS_SYNCING);
                        clrtoeol();
                        refresh();

                        if (g_cloudSyncEnabled && !g_currentRemote.empty()) {
                            syncToCloud(false); // foreground sync
                            mvprintw(LINES - 3, 0, TEXT_SETTINGS_SYNC_COMPLETE);
                        } else {
                            mvprintw(LINES - 3, 0, TEXT_SETTINGS_SYNC_DISABLED);
                        }
                        clrtoeol();
                        refresh();
                        sleep(2);
                        break;
                    }
                    case 6: { // Setup New Remote
                        def_prog_mode();
                        endwin();

                        std::cout << TEXT_SETTINGS_SETUP_HEADER << "\n";
                        std::cout << TEXT_SETTINGS_SETUP_RUNNING << "\n\n";
                        system("rclone config");

                        std::cout << TEXT_SETTINGS_SETUP_RETURN;
                        std::cin.get();

                        reset_prog_mode();
                        refresh();
                        break;
                    }
                    case 7: { // Fix/Reconnect Remote - Use new wizard
                        if (g_currentRemote.empty()) {
                            mvprintw(LINES - 3, 0, "No remote selected. Use option 3 to select a remote.");
                            clrtoeol();
                            refresh();
                            sleep(2);
                        } else {
                            showConnectionTest(); // This includes the fix wizard
                        }
                        break;
                    }
                    case 8: { // Test Connection
                        showConnectionTest();
                        break;
                    }
                    case 9: { // Change Sync Type
                        showSyncTypeMenu();
                        break;
                    }
                    case 10: // Back
                        inMenu = false;
                        break;
                }
                break;
            case 27: // ESC
            case 'q':
            case 'Q':
                inMenu = false;
                break;
        }
    }
}

// Function to draw a separator line across the full width of the screen
void drawSeparator(int y) {
    mvprintw(y, 0, "");
    for (int i = 0; i < COLS; i++) {
        addch('-');
    }
}

// Function to draw the UI
void drawUI(const std::vector<std::string>& files, int selectedIndex, int currentPage, const std::string& message = "") {
    clear();

    // Draw header with status on the same line
    attron(A_BOLD);
    mvprintw(0, 0, TEXT_HEADER);

    // Display status on the right side of the same line
    std::string statusText;
    if (!g_selectedPaths.empty()) {
        // Show selection count when items are selected
        statusText = std::to_string(g_selectedPaths.size()) + TEXT_MSG_ITEMS_SELECTED;
    } else {
        statusText = message.empty() ? TEXT_MSG_IDLE : message;
    }
    std::string statusDisplay = std::string(TEXT_STATUS_PREFIX) + statusText;
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

    drawSeparator(2);

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
                std::string displayName = files[i];
                std::string fullPath = getFullPath(displayName);
                bool isSelected = isPathSelected(fullPath);

                // Add '|' marker for all selected items
                std::string prefix = isSelected ? "| " : "  ";

                // Highlight current cursor
                if (i == selectedIndex) {
                    attron(A_REVERSE);
                }

                // Truncate filename if too long for column
                int maxWidth = COLUMN_WIDTH - 4;
                if ((int)displayName.length() > maxWidth) {
                    displayName = displayName.substr(0, maxWidth - 3) + "...";
                }

                // If item is selected but not the cursor, make it bold
                if (isSelected && i != selectedIndex) {
                    attron(A_BOLD);
                }

                mvprintw(yPos, xPos, "%s%s", prefix.c_str(), displayName.c_str());

                if (isSelected && i != selectedIndex) {
                    attroff(A_BOLD);
                }

                if (i == selectedIndex) {
                    attroff(A_REVERSE);
                }
            }
        }
    }

    // Draw footer with instructions
    int footerY = LINES - 5;
    drawSeparator(footerY);
    mvprintw(footerY + 1, 0, TEXT_CONTROLS);

    if (currentPath == BASE_DIR) {
        mvprintw(footerY + 2, 0, "  %s | %s | %c: %s | %c: %s | %c: %s | %s/%c: %s | %c: %s | %c: %s | %c: %s | %c: %s | %c: %s",
                 TEXT_ARROWS, TEXT_ENTER, KEY_NEW_FILE, TEXT_NEW_FILE_LABEL,
                 KEY_NEW_FOLDER, TEXT_NEW_FOLDER_LABEL, KEY_RENAME, TEXT_RENAME_LABEL,
                 TEXT_DELETE_KEY_LABEL, KEY_DELETE_ALT, TEXT_DELETE_LABEL,
                 KEY_CONVERT, TEXT_CONVERT_LABEL, KEY_SELECT_MODE, TEXT_SELECT_LABEL,
                 KEY_UNDO_ACTION, TEXT_UNDO_LABEL, KEY_SETTINGS, TEXT_SETTINGS_LABEL, KEY_QUIT, TEXT_QUIT_LABEL);
    } else {
        mvprintw(footerY + 2, 0, "  %s | %s | %c: %s | %c: %s | %c: %s | %c: %s | %s/%c: %s | %c: %s | %c: %s | %c: %s | %c: %s | %c: %s",
                 TEXT_ARROWS, TEXT_ENTER, KEY_BACK, TEXT_BACK_LABEL,
                 KEY_NEW_FILE, TEXT_NEW_FILE_LABEL, KEY_NEW_FOLDER, TEXT_NEW_FOLDER_LABEL,
                 KEY_RENAME, TEXT_RENAME_LABEL, TEXT_DELETE_KEY_LABEL, KEY_DELETE_ALT, TEXT_DELETE_LABEL,
                 KEY_CONVERT, TEXT_CONVERT_LABEL, KEY_SELECT_MODE, TEXT_SELECT_LABEL,
                 KEY_UNDO_ACTION, TEXT_UNDO_LABEL, KEY_SETTINGS, TEXT_SETTINGS_LABEL, KEY_QUIT, TEXT_QUIT_LABEL);
    }

    refresh();
}

// Function to get user input (string) with cancel support
std::string getUserInput(const std::string& prompt) {
    int y = LINES - 5; // Separator line position (above Controls)

    // Replace separator line with the prompt
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
                // Redraw the input line with prompt
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
        drawSeparator(2);

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

        drawSeparator(3 + templates.size() + 1);
        mvprintw(3 + templates.size() + 2, 0, TEXT_PROMPT_TEMPLATE_NAV);

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
void createFolder(const std::string& foldername) {
    std::string folderpath = currentPath + "/" + foldername;
    fs::create_directory(folderpath);
}

// Function to check if a file or folder already exists
bool fileExists(const std::string& name) {
    std::string actualName = name;

    // If checking for smart note with {S} prefix, add it to actual filename
    // (User input won't have {S}, but actual file does)
    // This function is called with user input, so we don't modify it

    std::string filepath = currentPath + "/" + actualName;
    return fs::exists(filepath);
}

// Function to delete a file or folder
void deleteFile(const std::string& filename) {
    std::string actualName = filename;

    // Remove [] prefix if present (folder)
    if (filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        actualName = filename.substr(2);
    }
    // Remove {S} prefix if present (smart note)
    else if (filename.length() >= 3 && filename.substr(0, 3) == TEXT_SMART_NOTE_LABEL) {
        actualName = "{S}" + filename.substr(3); // Keep {S} in actual filename
    }

    std::string filepath = currentPath + "/" + actualName;
    fs::remove_all(filepath); // remove_all handles both files and directories
}

// Function to rename a file or folder
void renameFile(const std::string& oldName, const std::string& newName) {
    std::string actualOldName = oldName;
    std::string actualNewName = newName;

    // Check if old name has [] prefix (folder)
    bool isFolder = (oldName.substr(0, 2) == TEXT_FOLDER_LABEL);
    // Check if old name has {S} prefix (smart note)
    bool isSmartNote = (oldName.length() >= 3 && oldName.substr(0, 3) == TEXT_SMART_NOTE_LABEL);

    if (isFolder) {
        actualOldName = oldName.substr(2);
    } else if (isSmartNote) {
        actualOldName = "{S}" + oldName.substr(3); // Actual file has {S} prefix
        // Keep {S} prefix in new name if it was a smart note
        if (newName.substr(0, 3) != "{S}") {
            actualNewName = "{S}" + newName;
        }
    }

    std::string oldPath = currentPath + "/" + actualOldName;
    std::string newPath = currentPath + "/" + actualNewName;
    fs::rename(oldPath, newPath);
}

// Function to open a file in micro editor
void openFileInEditor(const std::string& filename) {
    // Don't try to edit directories
    if (filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
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

// Function to convert text file to MLA-formatted DOCX
void convertMarkdownToDocx(const std::string& filename, std::string& statusMessage) {
    // Remove [] prefix if somehow present
    std::string actualFilename = filename;
    if (filename.substr(0, 2) == TEXT_FOLDER_LABEL) {
        statusMessage = TEXT_MSG_CANNOT_CONVERT_DIR;
        return;
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
