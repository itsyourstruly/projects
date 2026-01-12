#ifndef CLOUD_SYNC_H
#define CLOUD_SYNC_H

#include <string>
#include <vector>

// Cloud sync configuration management
void loadSettings();
void saveSettings();

// Cloud sync operations
void syncToCloud(bool silent = false);
void syncFromCloud();
bool testCloudConnection();

// Remote management
std::vector<std::string> listRcloneRemotes();

#endif // CLOUD_SYNC_H