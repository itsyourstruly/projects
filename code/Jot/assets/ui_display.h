#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include <string>
#include <vector>
#include <set>

// Selection state
extern std::set<std::string> g_selectedPaths;
extern bool g_selectMode;
extern int g_anchorIndex;
extern std::string g_anchorPath;

// Undo system
enum UndoType {
    UNDO_NONE,
    UNDO_CREATE_FILE,
    UNDO_CREATE_FOLDER,
    UNDO_DELETE,
    UNDO_RENAME,
    UNDO_MOVE
};

struct UndoAction {
    UndoType type;
    std::string path1;
    std::string path2;
    std::string content;
    bool wasDirectory;
    std::vector<std::string> movedPaths;
    std::vector<std::string> destPaths;
};

extern UndoAction g_lastAction;

// UI functions
void drawUI(const std::vector<std::string>& files, int selectedIndex, int currentPage, const std::string& message = "");
void drawSeparator(int y);
std::string getUserInput(const std::string& prompt);
int selectTemplate(const std::vector<std::string>& templates);

// Selection functions
bool isPathSelected(const std::string& fullPath);
bool performUndo(std::string& statusMessage);

// Search functions
void enterSearchMode(const std::vector<std::string>& allFiles, int& selectedIndex, std::string& statusMessage);

#endif // UI_DISPLAY_H

