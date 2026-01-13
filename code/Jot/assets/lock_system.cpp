#include "lock_system.h"
#include "config.h"
#include "file_operations.h"
#include "text.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::set<std::string> loadLocks(const std::string& directory) {
    std::set<std::string> locks;
    const std::string lockFile = directory + "/.jot_locks";

    // Fast path - if file doesn't exist, return empty set
    if (!fs::exists(lockFile)) {
        return locks;
    }

    std::ifstream file(lockFile, std::ios::in);
    if (file.is_open()) {
        std::string line;
        line.reserve(256); // Pre-allocate reasonable size
        while (std::getline(file, line)) {
            if (!line.empty()) {
                locks.insert(std::move(line));
            }
        }
    }
    return locks;
}

void saveLocks(const std::string& directory, const std::set<std::string>& locks) {
    const std::string lockFile = directory + "/.jot_locks";

    if (locks.empty()) {
        // Remove lock file if no locks remain (fast path)
        if (fs::exists(lockFile)) {
            fs::remove(lockFile);
        }
        return;
    }

    std::ofstream file(lockFile, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        for (const auto& lock : locks) {
            file << lock << '\n';
        }
        file.flush();
    }
}

bool isFileLocked(const std::string& directory, const std::string& filename) {
    auto locks = loadLocks(directory);
    return locks.find(filename) != locks.end();
}

void toggleFileLock(const std::string& displayFilename, std::string& statusMessage) {
    // Get the actual filename (without [] or {S} prefix)
    std::string actualName = getActualFilename(displayFilename);

    auto locks = loadLocks(currentPath);
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


