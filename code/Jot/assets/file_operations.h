#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>

// Directory setup
void ensureDirectories();

// File listing and management
std::vector<std::string> getFiles();
std::vector<std::string> getTemplates();
std::vector<std::string> getAllFilesRecursive(const std::string& basePath);
std::string getFullPath(const std::string& filename);
std::string getActualName(const std::string& displayName);
std::string getActualFilename(const std::string& displayName);

// File operations
bool fileExists(const std::string& name);
void createFolder(const std::string& name);
void createFileWithTemplate(const std::string& filename, const std::string& templateName);
void deleteFile(const std::string& filename);
void renameFile(const std::string& oldName, const std::string& newName);
void openFileInEditor(const std::string& filename);
void convertMarkdownToDocx(const std::string& filename, std::string& statusMessage);

// Time utilities
std::string getTimeAgo(const std::string& filepath);

#endif // FILE_OPERATIONS_H