#ifndef LOCK_SYSTEM_H
#define LOCK_SYSTEM_H

#include <string>
#include <set>

// Lock management functions
std::set<std::string> loadLocks(const std::string& directory);
void saveLocks(const std::string& directory, const std::set<std::string>& locks);
bool isFileLocked(const std::string& directory, const std::string& filename);
void toggleFileLock(const std::string& displayFilename, std::string& statusMessage);
bool isFileLockedByDisplay(const std::string& displayFilename);

#endif // LOCK_SYSTEM_H