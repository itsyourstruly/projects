#include "cloud_sync.h"
#include "config.h"
#include "text.h"
#include <cstdlib>
#include <fstream>
#include <string>

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

void syncToCloud(bool silent) {
    if (!g_cloudSyncEnabled || g_currentRemote.empty()) return;

    std::string cmd;

    if (g_syncType == SYNC_TYPE_FULL) {
        cmd = "rclone sync \"" + BASE_DIR + "\" \"" + g_currentRemote + ":" + RCLONE_PATH +
              "\" --exclude '.git/**' 2>/dev/null";
    } else {
        cmd = "rclone copy \"" + BASE_DIR + "\" \"" + g_currentRemote + ":" + RCLONE_PATH +
              "\" --exclude '.git/**' 2>/dev/null";
    }

    if (silent) {
        cmd += " &";
    }

    system(cmd.c_str());
}

void syncFromCloud() {
    if (!g_cloudSyncEnabled || g_currentRemote.empty()) return;

    std::string cmd = "rclone copy \"" + g_currentRemote + ":" + RCLONE_PATH + "\" \"" + BASE_DIR +
                      "\" --exclude '.git/**' 2>/dev/null";

    system(cmd.c_str());
}

bool testCloudConnection() {
    if (g_currentRemote.empty()) return false;

    std::string cmd = "rclone lsd \"" + g_currentRemote + ":\" --max-depth 1 > /dev/null 2>&1";
    int result = system(cmd.c_str());
    return (result == 0);
}

std::vector<std::string> listRcloneRemotes() {
    std::vector<std::string> remotes;
    FILE* pipe = popen("rclone listremotes 2>/dev/null", "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            std::string remote = buffer;
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

