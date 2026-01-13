#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>

// Function to handle all keyboard input and update state accordingly
void handleInput(int ch, std::vector<std::string>& files, int& selectedIndex,
                 int& currentPage, std::string& statusMessage, bool& running,
                 int maxFilesPerColumn, int filesPerPage, int totalPages, bool& needsRefresh);

#endif // INPUT_HANDLER_H

