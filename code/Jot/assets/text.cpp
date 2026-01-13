#include "text.h"

// UI Text
const char* TEXT_HEADER = "Happy Writing!!";
const char* TEXT_SEPARATOR = "--------------------------------------";
const char* TEXT_HOME_FOLDER = "Presenting the home";
const char* TEXT_YOU_ARE_IN = "Presenting %s";
const char* TEXT_PAGE_INDICATOR = "Page %d of %d";
const char* TEXT_NO_FILES = "empty canvas";
const char* TEXT_CONTROLS = "Controls:";
const char* TEXT_FOLDER_LABEL = "[]";
const char* TEXT_SMART_NOTE_LABEL = "{S}";
const char* TEXT_LOCKED_PREFIX = "[L]";
const char* TEXT_STATUS_PREFIX = "Status - ";

// Control labels
const char* TEXT_ARROWS = "arrows: Navigate";
const char* TEXT_ENTER = "enter: Open";
const char* TEXT_NEW_FILE_LABEL = "New note";
const char* TEXT_NEW_FOLDER_LABEL = "New Folder";
const char* TEXT_RENAME_LABEL = "Retitle";
const char* TEXT_DELETE_LABEL = "Scrap";
const char* TEXT_DELETE_KEY_LABEL = "DEL";
const char* TEXT_CONVERT_LABEL = "Convert";
const char* TEXT_BACK_LABEL = "Back";
const char* TEXT_QUIT_LABEL = "Leave";
const char* TEXT_SELECT_LABEL = "Choose";
const char* TEXT_UNDO_LABEL = "Rewind";
const char* TEXT_SETTINGS_LABEL = "Settings";
const char* TEXT_LOCK_LABEL = "Lock";

// Settings menu text
const char* TEXT_SETTINGS_HEADER = "Cloud Sync Settings";
const char* TEXT_SETTINGS_CURRENT_REMOTE = "Current Remote: ";
const char* TEXT_SETTINGS_CLOUD_SYNC = "Cloud Sync: ";
const char* TEXT_SETTINGS_AUTO_SYNC = "Auto Sync: ";
const char* TEXT_SETTINGS_ENABLED = "enabled";
const char* TEXT_SETTINGS_DISABLED = "disabled";
const char* TEXT_SETTINGS_ON = "on";
const char* TEXT_SETTINGS_OFF = "off (manual)";
const char* TEXT_SETTINGS_NONE = "(none)";
const char* TEXT_SETTINGS_OPTIONS = "options:";
const char* TEXT_SETTINGS_NAV = "Use arrow keys to navigate, enter to select, escape to withdraw";
const char* TEXT_SETTINGS_OPT_VIEW = "1. View Remotes";
const char* TEXT_SETTINGS_OPT_STATUS = "2. Sync Status";
const char* TEXT_SETTINGS_OPT_SWITCH = "3. Switch Remote";
const char* TEXT_SETTINGS_OPT_TOGGLE_CLOUD = "4. Toggle Cloud Sync (Enable/Disable)";
const char* TEXT_SETTINGS_OPT_TOGGLE_AUTO = "5. Toggle Auto Sync Mode";
const char* TEXT_SETTINGS_OPT_SYNC_NOW = "6. Sync Now (Manual)";
const char* TEXT_SETTINGS_OPT_SETUP = "7. Setup New Remote";
const char* TEXT_SETTINGS_OPT_BACK = "8. Withdraw to Files";
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
const char* TEXT_SETTINGS_CONFIG_FILE = "jot_config.txt";
const char* TEXT_SETTINGS_TESTING_CONNECTION = "Testing connection...";
const char* TEXT_SETTINGS_CONNECTION_OK = "✓ Connection successful!";
const char* TEXT_SETTINGS_CONNECTION_FAILED = " Connection failed - Remote needs reconfiguration";
const char* TEXT_SETTINGS_CONNECTION_HELP = "Run 'rclone config' to fix authentication issues";
const char* TEXT_SETTINGS_FIX_REMOTE = "9. Fix/Reconnect Remote";
const char* TEXT_SETTINGS_OPT_TEST_CONNECTION = "10. Test Connection";
const char* TEXT_SETTINGS_OPT_SYNC_TYPE = "11. Change Sync Type";
const char* TEXT_SETTINGS_OPT_BACK_NEW = "12. Back to Files";

// Sync type constants
const char* TEXT_SYNC_TYPE_FULL = "Full Sync (bidirectional)";
const char* TEXT_SYNC_TYPE_ONE_WAY = "One Way (upload only)";
const char* TEXT_SETTINGS_SYNC_TYPE = "Sync Type: ";

// Connection test messages
const char* TEXT_CONNECTION_TEST_HEADER = "Connection Test";
const char* TEXT_CONNECTION_TEST_TESTING = "Testing connection to: ";
const char* TEXT_CONNECTION_TEST_SUCCESS = "✓ SUCCESS! Connection is working perfectly.";
const char* TEXT_CONNECTION_TEST_FAILED = " FAILED! Cannot connect to remote.";
const char* TEXT_CONNECTION_TEST_NO_REMOTE = "No remote selected. Please select a remote first (Option 3).";
const char* TEXT_CONNECTION_TEST_SYNC_DISABLED = "Cloud sync is disabled. Enable it first (Option 4).";
const char* TEXT_CONNECTION_FIX_PROMPT = "Would you like to fix this connection? (y/n): ";
const char* TEXT_CONNECTION_PRESS_KEY = "Press any key to continue...";

// Fix wizard messages
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
const char* TEXT_FIX_WIZARD_FAILED = "Fix failed. You may need to delete and recreate the remote.";
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
const char* TEXT_SYNC_TYPE_CHOOSE = "Choose type:";

// Prompts
const char* TEXT_PROMPT_NEW_FILE = "Enter the note's title (ESC to cancel): ";
const char* TEXT_PROMPT_NEW_FOLDER = "Enter the folder's title (ESC to cancel): ";
const char* TEXT_PROMPT_RENAME = "Enter new name for '%s' (ESC to cancel): ";
const char* TEXT_PROMPT_DELETE_CONFIRM = "scrap %s? (y/n): ";
const char* TEXT_PROMPT_SELECT_TEMPLATE = "Select a Canvas";
const char* TEXT_PROMPT_TEMPLATE_NAV = "Use arrow keys to navigate, ENTER to select, ESC to cancel";

// Status messages
const char* TEXT_MSG_IDLE = "Available";
const char* TEXT_MSG_OPENED_FOLDER = "Entered folder ";
const char* TEXT_MSG_EDITED_FILE = "Altered ";
const char* TEXT_MSG_RETURNED = "Withdrew to from folder";
const char* TEXT_MSG_CREATED_FILE = "New note ";
const char* TEXT_MSG_CREATED_FOLDER = "New folder ";
const char* TEXT_MSG_USER_CANCELLED = "Withdrew choice";
const char* TEXT_MSG_ALREADY_EXISTS = "name already exists.";
const char* TEXT_MSG_RENAMED = "Retitled to ";
const char* TEXT_MSG_RENAME_CANCELLED = "Kept title";
const char* TEXT_MSG_NO_FILES_RENAME = "No notes to retitle.";
const char* TEXT_MSG_DELETED = "Scrapped ";
const char* TEXT_MSG_DELETE_CANCELLED = "Kept record";
const char* TEXT_MSG_NO_FILES_DELETE = "No files to scrap.";
const char* TEXT_MSG_LOCKED = "Locked ";
const char* TEXT_MSG_UNLOCKED = "Unlocked ";
const char* TEXT_MSG_CANNOT_DELETE_LOCKED = "Cannot scrap locked file";
const char* TEXT_MSG_CANNOT_RENAME_LOCKED = "Cannot retitle locked file";
const char* TEXT_MSG_CANNOT_MOVE_LOCKED = "Cannot relocate locked files";

// Selection messages
const char* TEXT_MSG_ITEMS_SELECTED = " item(s) chosen ('esc' to deselect, 'v' to relocate)";
const char* TEXT_MSG_SELECTION_MODE_ON = "Selection mode ON (press 's' again to lock in selection, or esc to withdraw)";
const char* TEXT_MSG_SELECTION_LOCKED = "Selection locked (";
const char* TEXT_MSG_SELECTION_CANCELLED = "Selection withdrew";
const char* TEXT_MSG_NO_ITEMS_TO_MOVE = "No assets chosen to move";

// Move operation messages
const char* TEXT_MSG_MOVED_ITEMS = "Relocated ";
const char* TEXT_MSG_MOVE_FAILED = " failed)";
const char* TEXT_MSG_MOVE_ERROR = "Relocation failed";
const char* TEXT_MSG_NO_ITEMS_MOVE = "No assets to move";

// Undo messages
const char* TEXT_MSG_NOTHING_TO_UNDO = "No actions to rewind";
const char* TEXT_MSG_UNDONE = "Rewound ";
const char* TEXT_MSG_UNDO_FAILED = "Rewind failed";

// Conversion messages
const char* TEXT_MSG_CONVERTING = "Converting to Word document with MLA formatting...";
const char* TEXT_MSG_CONVERTED_TO = "Converted to ";
const char* TEXT_MSG_MLA_APPLIED = " (MLA formatting applied)";
const char* TEXT_MSG_CONVERSION_FAILED = "Conversion failed";
const char* TEXT_MSG_FILE_MUST_BE_MD_TXT = "note must be .md or .txt format";
const char* TEXT_MSG_CANNOT_CONVERT_DIR = "Cannot convert a directory";
const char* TEXT_MSG_NO_FILE_FOR_CONVERSION = "No file chosen for conversion";

// Template messages
const char* TEXT_MSG_BLANK_LABEL = "empty";

// Search/Find messages
const char* TEXT_SEARCH_PROMPT = "Find: ";
const char* TEXT_SEARCH_RESULTS = " items discovered. (X - esc, peek - e, view - v)";
const char* TEXT_SEARCH_NO_RESULTS = "No matches found";

// Cloud sync messages
const char* TEXT_MSG_OPENED = "Entered ";