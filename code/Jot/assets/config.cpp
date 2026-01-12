#include "config.h"

// Directory configuration
const std::string BASE_DIR = "files";
const std::string TEMPLATES_DIR = "templates";
std::string currentPath = BASE_DIR;

// Cloud sync configuration (rclone)
const bool ENABLE_CLOUD_SYNC = true;
const std::string RCLONE_REMOTE = "Google Drive";
const std::string RCLONE_PATH = "jot/files";
const int SYNC_DELAY_SECONDS = 2;

// Runtime settings
std::string g_currentRemote = RCLONE_REMOTE;
bool g_cloudSyncEnabled = ENABLE_CLOUD_SYNC;
bool g_autoSyncEnabled = true;
SyncType g_syncType = SYNC_TYPE_ONE_WAY;

// UI Layout configuration
const int COLUMN_WIDTH = 30;
const int MAX_COLUMNS = 3;
const int TOP_OFFSET = 3;
const int BOTTOM_OFFSET = 5;