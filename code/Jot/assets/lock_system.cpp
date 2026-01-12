#include "lock_system.h"
#include "config.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::set<std::string> loadLocks(const std::string& directory) {
    std::set<std::string> locks;
    std::string lockFile = directory + "/.jot_locks";

    if (fs::exists(lockFile)) {
        std::ifstream file(lockFile);
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                locks.insert(line);
            }
        }
        file.close();
    }
    return locks;
}

void saveLocks(const std::string& directory, const std::set<std::string>& locks) {
    std::string lockFile = directory + "/.jot_locks";

    if (locks.empty()) {
        // Remove lock file if no locks remain
        if (fs::exists(lockFile)) {
            fs::remove(lockFile);
        }
        return;
    }

    std::ofstream file(lockFile);
    for (const auto& lock : locks) {
        file << lock << "\n";
    }
    file.close();
}

bool isFileLocked(const std::string& directory, const std::string& filename) {
    auto locks = loadLocks(directory);
    return locks.find(filename) != locks.end();
}

// Note: toggleFileLock and isFileLockedByDisplay are in main.cpp
// because they depend on getActualFilename which is also in main.cpp


