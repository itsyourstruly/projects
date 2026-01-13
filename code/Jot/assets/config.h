#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Directory configuration
extern const std::string BASE_DIR;
extern const std::string TEMPLATES_DIR;
extern std::string currentPath;

// Cloud sync configuration (rclone)
extern const bool ENABLE_CLOUD_SYNC;
extern const std::string RCLONE_REMOTE;
extern const std::string RCLONE_PATH;
extern const int SYNC_DELAY_SECONDS;

// Runtime settings (loaded from config file, can be changed in settings menu)
extern std::string g_currentRemote;
extern bool g_cloudSyncEnabled;
extern bool g_autoSyncEnabled;

// Sync type enum
enum SyncType {
    SYNC_TYPE_FULL = 0,      // Bidirectional - mirrors changes both ways
    SYNC_TYPE_ONE_WAY = 1    // One-way - only uploads, never deletes from cloud
};
extern SyncType g_syncType;

// UI Layout configuration
extern const int COLUMN_WIDTH;
extern const int MAX_COLUMNS;
extern const int TOP_OFFSET;
extern const int BOTTOM_OFFSET;

#endif // CONFIG_H// Test change
