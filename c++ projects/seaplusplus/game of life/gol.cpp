#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <unistd.h>
using std::cout;
using std::cin;
using std::string;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::istringstream;

// Game constants
const char ALIVE = 'X';
const char NOTALIVE = ' ';
const int PROB_ALIVE = 1; // probability of a cell being alive out of 10
const char SWITCH = 'S';  // R will Run for ever , S will stop at each generation

// String constants
const string promptEnterFileName = "Enter filename to read from (e.g., example.txt): ";
const string promptSaveFileName = "Enter filename to save to (e.g., output.txt): ";
const string errorCouldNotOpenFile = "Error: Could not open file ";
const string errorCouldNotCreateFile = "Error: Could not create file ";
const string errorEmptyFile = "Error: Empty file. Creating empty grid.\n";
const string errorInvalidFirstLine = "Error: Invalid first line format. Creating empty grid.\n";
const string messageCreatingEmptyGrid = ". Creating empty grid.\n";
const string messageGridSavedSuccessfully = " successfully.\n";
const string messageGridSaved = "Grid saved to ";
const string promptEnterRows = "Enter number of rows: ";
const string promptEnterColumns = "Enter number of columns: ";
const string messageInvalidDimensions = "Invalid dimensions. Using default 10x10.\n";
const string instructionsEnterGrid = "Enter the grid row by row. Use '";
const string instructionsForAliveCells = "' for alive cells, any other character for dead cells.\n";
const string instructionsEachRow = "Each row should have ";
const string instructionsCharacters = " characters (spaces count as dead cells).\n";
const string instructionsPressEnter = "Press Enter after each row.\n\n";
const string promptRow = "Row ";
const string promptRowColon = ": ";
const string messageGridCreated = "\nGrid created successfully!\n";
const string gameTitle = "Conway's Game of Life\n";
const string menuOption1 = "1. Read grid from file\n";
const string menuOption2 = "2. Use default grid\n";
const string menuOption3 = "3. Input grid manually\n";
const string promptMenuChoice = "Enter your choice (1-3): ";
const string messageInvalidChoice = "Invalid choice, using default grid.\n";
const string commandClear = "clear";
const string promptContinueOrExit = "Press Enter to continue, 's' to save grid, 'x' to exit: ";
const string fileInstructions = "Note: Files should be in the same directory as this program.\n";


// function prototypes
char lifeIsRandom();      // give an 'X' or ' ' randomly
char **makeStartingGrid(int rows, int columns);    // make the starting grid
void deleteGrid(char **grid, int rows);     // free up memory used by grid
void showGrid(char **grid, int rows, int columns);   // show the grid
char **nextGeneration(char **current, int rows, int columns);     // takes current grid and makes next generation
int countLiveNeighbors(char **current, int x, int y, int maxX, int maxY);   // count live neighbors around a cell
char checkSquare(char **current, int x, int y, int maxX, int maxY);      // check current square to see if alive or not alive in next gen.
char **barGrid(int ROWS, int COLUMNS);    // make a grid with a bar;
char **readInAGrid(string filePath, int &rows, int &columns);      // will read in a grid from a file (csv or txt)
bool writeGridToFile(char **grid, string filePath, int rows, int columns);  // will write the current grid to a file(csv or txt)
char **userInputGrid(int &rows, int &columns);     // Allow the user to type in a grid one row at a time
int showMenu();     // Show menu and get user choice


int main()
{
    // Set up the game board size
    int ROWS = 30;
    int COLUMNS = 80;
    
    // Initialize random number generator for random grids
    srand((unsigned)time(NULL));
    
    // This will hold our current game grid
    char **currentGrid;
    
    // Counter to reset grid after 25 generations
    int counter = 0;
    
    // Show menu and get user's choice
    int choice = showMenu();
    
    // Create the starting grid based on user choice
    switch(choice) {
        case 1: {
            // Read grid from a file
            cout << fileInstructions;
            cout << promptEnterFileName;
            string filename;
            cin >> filename;
            currentGrid = readInAGrid(filename, ROWS, COLUMNS);
            break;
        }
        case 2:
            // Create a random grid
            currentGrid = makeStartingGrid(ROWS, COLUMNS);
            break;
        case 3:
            // Let user input their own grid
            currentGrid = userInputGrid(ROWS, COLUMNS);
            break;
        default:
            // Invalid choice - use random grid as default
            cout << messageInvalidChoice;
            currentGrid = makeStartingGrid(ROWS, COLUMNS);
            break;
    }
    
    // Main game loop - runs forever
    while (true)
    {
        // Clear the screen for animation effect
        system(commandClear.c_str()); 
        
        // Show the current generation
        showGrid(currentGrid, ROWS, COLUMNS);
        
        // Wait 100ms between generations
        usleep(100000); 
        
        // Calculate the next generation using Conway's rules
        char **nextGen = nextGeneration(currentGrid, ROWS, COLUMNS);
        
        // Free memory from current generation
        deleteGrid(currentGrid, ROWS);
        
        // Update to the new generation
        currentGrid = nextGen;
        
        // Check if we should pause for user input
        if (SWITCH == 'S')
        { 
            // Ask user what to do next
            cout << promptContinueOrExit;
            char input;
            input = getchar();
            
            // Exit if user pressed 'x'
            if ((input == 'x') || (input == 'X'))
            {
                return 0;
            }
            // Save grid if user pressed 's'
            else if ((input == 's') || (input == 'S'))
            {
                cout << promptSaveFileName;
                string filename;
                cin >> filename;
                writeGridToFile(currentGrid, filename, ROWS, COLUMNS);
                cin.ignore(); 
            }
        }
        
        // Count generations
        counter++;
        
        // Reset with new random grid after 25 generations
        if (counter > 25)
        {
            counter = 0;
            deleteGrid(currentGrid, ROWS);
            currentGrid = makeStartingGrid(ROWS, COLUMNS);
        }
    }
    return 0;
}
// Read a grid from a text file
char **readInAGrid(string filePath, int &rows, int &columns)
{
    // Try to open the file
    ifstream file(filePath);
    if (!file.is_open()) {
        // If file won't open, create empty grid instead
        cout << errorCouldNotOpenFile << filePath << messageCreatingEmptyGrid;
        rows = 10;
        columns = 10;
        char **grid = new char*[rows];
        for (int i = 0; i < rows; i++) {
            grid[i] = new char[columns];
            for (int j = 0; j < columns; j++) {
                grid[i][j] = NOTALIVE;  // Fill with dead cells
            }
        }
        return grid;
    }
    
    // Read the first line to get grid size
    string firstLine;
    if (!getline(file, firstLine)) {
        // If file is empty, create empty grid
        cout << errorEmptyFile;
        rows = 10;
        columns = 10;
        char **grid = new char*[rows];
        for (int i = 0; i < rows; i++) {
            grid[i] = new char[columns];
            for (int j = 0; j < columns; j++) {
                grid[i][j] = NOTALIVE;
            }
        }
        file.close();
        return grid;
    }
    
    // Parse the first line to get rows and columns (format: "rows,columns")
    istringstream iss(firstLine);
    string rowStr, colStr;
    if (getline(iss, rowStr, ',') && getline(iss, colStr)) {
        rows = atoi(rowStr.c_str());      // Convert string to number
        columns = atoi(colStr.c_str());   // Convert string to number
    } else {
        // If first line format is wrong, use default size
        cout << errorInvalidFirstLine;
        rows = 10;
        columns = 10;
    }
    
    // Make sure we have valid dimensions
    if (rows <= 0 || columns <= 0) {
        rows = 10;
        columns = 10;
    }
    
    // Create the grid in memory
    char **grid = new char*[rows];
    for (int i = 0; i < rows; i++) {
        grid[i] = new char[columns];
        for (int j = 0; j < columns; j++) {
            grid[i][j] = NOTALIVE; // Start with all cells dead
        }
    }
    
    // Read each line of the file and fill the grid
    string line;
    int currentRow = 0;
    while (getline(file, line) && currentRow < rows) {
        // Split the line by commas
        istringstream lineStream(line);
        string cell;
        int currentCol = 0;
        
        while (getline(lineStream, cell, ',') && currentCol < columns) {
            // Remove extra spaces from the cell data
            while (!cell.empty() && cell[0] == ' ') cell.erase(0, 1);
            while (!cell.empty() && cell.back() == ' ') cell.pop_back();
            
            // Check if this cell should be alive
            if (!cell.empty() && (cell[0] == ALIVE || cell[0] == 'x' || cell[0] == 'X')) {
                grid[currentRow][currentCol] = ALIVE;
            } else {
                grid[currentRow][currentCol] = NOTALIVE;
            }
            currentCol++;
        }
        currentRow++;
    }
    
    file.close();
    return grid;
}
// Save the current grid to a text file
bool writeGridToFile(char **grid, string filePath, int rows, int columns)
{
    // Try to create the output file
    ofstream file(filePath);
    if (!file.is_open()) {
        cout << errorCouldNotCreateFile << filePath << "\n";
        return false;
    }
    
    // Write the grid size on the first line
    file << rows << "," << columns << "\n";
    
    // Write each row of the grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // Add comma between cells (except for first cell)
            if (j > 0) file << ",";
            
            // Write the cell (alive or dead)
            if (grid[i][j] == ALIVE) {
                file << ALIVE;
            } else {
                file << " ";
            }
        }
        file << "\n";  // End of row
    }
    
    file.close();
    cout << messageGridSaved << filePath << messageGridSavedSuccessfully;
    return true;
}
// Let the user create a grid by typing it in
char **userInputGrid(int &rows, int &columns)
{
    // Get the size of grid from user
    cout << promptEnterRows;
    cin >> rows;
    cout << promptEnterColumns;
    cin >> columns;
    
    // Make sure dimensions are valid
    if (rows <= 0 || columns <= 0) {
        cout << messageInvalidDimensions;
        rows = 10;
        columns = 10;
    }
    
    // Create an empty grid
    char **grid = new char*[rows];
    for (int i = 0; i < rows; i++) {
        grid[i] = new char[columns];
        for (int j = 0; j < columns; j++) {
            grid[i][j] = NOTALIVE; // Start with all cells dead
        }
    }
    
    // Give user instructions
    cout << instructionsEnterGrid << ALIVE << instructionsForAliveCells;
    cout << instructionsEachRow << columns << instructionsCharacters;
    cout << instructionsPressEnter;
    
    cin.ignore(); // Clear the input buffer
    
    // Get each row from the user
    for (int i = 0; i < rows; i++) {
        cout << promptRow << (i + 1) << promptRowColon;
        string row;
        getline(cin, row);
        
        // Fill the grid row with user input
        for (int j = 0; j < columns && j < (int)row.length(); j++) {
            if (row[j] == ALIVE || row[j] == 'x' || row[j] == 'X') {
                grid[i][j] = ALIVE;  // Make cell alive
            } else {
                grid[i][j] = NOTALIVE;  // Make cell dead
            }
        }
        
        // If user typed less characters than columns, remaining cells stay dead
    }
    
    cout << messageGridCreated;
    return grid;
}
// Create a test grid with a horizontal bar pattern
char **barGrid(int ROWS, int COLUMNS)
{
    // Create the grid
    char **outval = new char *[ROWS];
    
    // Pattern for the bar (3 alive cells, then 2 dead cells)
    char barRow[] = {ALIVE, ALIVE, ALIVE, NOTALIVE, NOTALIVE};
    
    for (int index = 0; index < ROWS; index++)
    {
        // Create each row
        char *current = new char[COLUMNS];
        
        if (index == 1)  // Put the bar on the second row
        {
            for (int i = 0; i < COLUMNS; i++)
            {
                if (i < 5) {
                    current[i] = barRow[i];  // Use the bar pattern
                } else {
                    current[i] = NOTALIVE;   // Fill rest with dead cells
                }
            } 
        } 
        else  // All other rows are empty
        {
            for (int i = 0; i < COLUMNS; i++)
            {
                current[i] = NOTALIVE;  // Fill with dead cells
            } 
        } 
        outval[index] = current;
    } 
    return outval;
}
// Calculate the next generation using Conway's Game of Life rules
char **nextGeneration(char **current, int rows, int columns)
{
    // Create a new grid for the next generation
    char **outval = new char *[rows];
    
    // Go through each cell and apply the rules
    for (int index = 0; index < rows; index++)
    {
        outval[index] = new char[columns];
        for (int jndex = 0; jndex < columns; jndex++)
        {
            // Check what this cell should be in the next generation
            outval[index][jndex] = checkSquare(current, index, jndex, rows, columns);
        } 
    } 
    return outval;
}

// Count how many live neighbors a cell has
int countLiveNeighbors(char **current, int x, int y, int maxX, int maxY)
{
    int count = 0;
    
    // Check all 8 cells around this one
    for (int rdex = x - 1; rdex <= x + 1; rdex++)
    {
        // Make sure we're not outside the grid
        if (rdex >= 0 && rdex < maxX)
        {
            for (int cdex = y - 1; cdex <= y + 1; cdex++)
            {
                // Make sure we're not outside the grid
                if (cdex >= 0 && cdex < maxY)
                {
                    // Don't count the cell itself, only its neighbors
                    if (!(rdex == x && cdex == y))
                    {
                        if (current[rdex][cdex] == ALIVE)
                        {
                            count++;  // Found a live neighbor
                        }
                    }
                } 
            } 
        } 
    } 
    return count;
}
// Apply Conway's Game of Life rules to determine if a cell lives or dies
char checkSquare(char **current, int x, int y, int maxX, int maxY)
{
    // Count how many live neighbors this cell has
    int liveNeighbors = countLiveNeighbors(current, x, y, maxX, maxY);
    
    // Check if the cell is currently alive or dead
    bool isCurrentlyAlive = current[x][y] == ALIVE;
    
    // Apply Conway's Game of Life rules:
    if (isCurrentlyAlive) {
        // RULE 1: A live cell with fewer than 2 live neighbors dies (underpopulation)
        if (liveNeighbors < 2) {
            return NOTALIVE;
        }
        // RULE 2: A live cell with 2 or 3 live neighbors stays alive
        else if (liveNeighbors == 2 || liveNeighbors == 3) {
            return ALIVE;
        }
        // RULE 3: A live cell with more than 3 live neighbors dies (overcrowding)
        else {
            return NOTALIVE;
        }
    }
    else {
        // RULE 4: A dead cell with exactly 3 live neighbors becomes alive (reproduction)
        if (liveNeighbors == 3) {
            return ALIVE;
        }
        else {
            return NOTALIVE;  // Stay dead
        }
    }
}
// Display the grid on screen
void showGrid(char **grid, int rows, int columns)
{
    // Print each row
    for (int rdex = 0; rdex < rows; rdex++)
    {
        // Print each cell in the row
        for (int cdex = 0; cdex < columns; cdex++)
        {
            cout << grid[rdex][cdex];  // Print 'X' or ' '
        }
        cout << '\n';  // Go to next line after each row
    }
    usleep(100000); // Wait 100ms so animation is visible
}

// Create a new grid filled with random alive/dead cells
char **makeStartingGrid(int rows, int columns)
{
    // Create the grid structure
    char **outval = new char *[rows];
    
    // Fill each cell randomly
    for (int index = 0; index < rows; index++)
    {
        outval[index] = new char[columns];
        for (int jndex = 0; jndex < columns; jndex++)
        {
            // Randomly decide if this cell is alive or dead
            outval[index][jndex] = lifeIsRandom();
        } 
    } 
    return outval;
}
// Free up memory used by a grid
void deleteGrid(char **grid, int rows)
{
    // Delete each row
    for (int index = 0; index < rows; index++)
    {
        if (grid[index] != NULL)
        {
            delete[] grid[index];  // Free memory for this row
        } 
    } 
    
    // Delete the array of row pointers
    if (grid != NULL)
    {
        delete[] grid;
    }
}

// Randomly decide if a cell should be alive or dead
char lifeIsRandom()
{
    // Get a random number from 1 to 10
    int value = 1 + (rand() % 10);
    
    // If the number is small enough, make the cell alive
    if (value <= PROB_ALIVE)
    {
        return ALIVE;  // Return 'X'
    }
    return NOTALIVE;   // Return ' ' (space)
}

// Show the main menu and get user's choice
int showMenu()
{
    int choice;
    
    // Display the menu options
    cout << gameTitle;
    cout << menuOption1;
    cout << menuOption2;
    cout << menuOption3;
    cout << promptMenuChoice;
    
    // Get user's choice
    cin >> choice;
    return choice;
}
