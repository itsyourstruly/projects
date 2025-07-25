#include <iostream>
#include <vector>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

const char borderChar = '#';
const char snakeChar = 'O';
const char fruitChar = 'F';
const char tailChar = 'o';


const char upInput = 'w';
const char upInput2 = 'W';
const int upInput3 = KEY_UP;

const char downInput = 's';
const char downInput2 = 'S';
const int downInput3 = KEY_DOWN;

const char leftInput = 'a';
const char leftInput2 = 'A';
const int leftInput3 = KEY_LEFT;

const char rightInput = 'd';
const char rightInput2 = 'D';
const int rightInput3 = KEY_RIGHT;

const char exitInput = 'x';
const char exitInput2 = 'X';

const char shopInput = 'e';
const char shopInput2 = 'E';


const char quitMessage[] = "Press 'X' to quit the game.";
const char shopMessage[] = "Press 'E' to open the shop.";
const char scoreMessage[] = "Score: ";
const char finalScoreMessage[] = "Final Score: ";
const char exitMessage[] = "Press any key to exit.";
const char welcomeMessage[] = "Welcome to the Snake Game!";
const char instructionsMessage[] = "Use W, A, S, D to move the snake.";
const char gameOverMessage[] = "Game Over!";


const char shopMenuMessage[] = "welcome to the shop";
const char shopMenuInstructions[] = "move with up and down keys to navigate, press enter to select an item. Exit with X.";
const char itemOwnedMessage[] = "You already own this item!";
const char itemNotEnoughPointsMessage[] = "Not enough points to buy this item!";
const char itemPurchasedMessage[] = "You bought %s!";

const char* itemNames[] = {"Speed Boost", "Slow Motion", "Invincible"};
const int itemPrices[] = {5, 5, 10};
bool itemOwned[] = {false, false, false};
bool itemActive[] = {false, false, false}; // Track if items are active
const int numItems = 3;
int selectedItem = 0;  // Currently highlighted item (0, 1, or 2)
int gameSpeed = 100000;  // Default game speed (in microseconds)

const int width = 60;
const int height = 10;

int x, y, score;
int fruitAmount = 10;
vector<int> fruitx, fruity; 
vector<int> tailx, taily;
int nTail;

enum Direction {
    Stop = 0,
    Left,
    Right,
    Up,
    Down
};
Direction dir;

bool gameOver;
bool shopOpen = false;

//void prototypes
void setup();
void draw();
void input();
void Logic();
void shop();

int main() {
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo typed characters
    keypad(stdscr, TRUE); // Enable special keys
    srand(time(0));
    printw("%s\n%s", welcomeMessage, instructionsMessage);
    refresh();
    nodelay(stdscr, TRUE); // Set non-blocking input mode
    for (int i = 0; i < 30; i++) {
        if (getch() != ERR) {
            break; // Exit the welcome screen if any key is pressed
        }
        usleep(100000); // Sleep for 100 milliseconds
    }
    setup();
    while (!gameOver) {
        draw();
        input();
        Logic();
        if (shopOpen) {
            shop();
        }

        // Handle boundary wrapping
        if (x >= width)
            x = 0;
        else if (x < 0)
            x = width - 1;
        if (y >= height)
            y = 0;
        else if (y < 0)
            y = height - 1;
            
        // Check collision with tail (only if not invincible)
        if (!itemActive[2]) { // Only check collision if invincibility is not active
            for (int i = 0; i < nTail; i++) {
                if (tailx[i] == x && taily[i] == y) {
                    gameOver = true;
                }
            }
        }
        
        // Check if fruit is eaten
        for (int i = 0; i < fruitx.size(); i++) {
            if (x == fruitx[i] && y == fruity[i]) {
                score += 1;
                // Replace eaten fruit with new random position
                fruitx[i] = rand() % width;
                fruity[i] = rand() % height;
                nTail++;
                tailx.resize(nTail);
                taily.resize(nTail);
                break;  // Only eat one fruit per move
            }
        }
        int currentSpeed = gameSpeed;
        usleep(currentSpeed); // Sleep for the current game speed
    }
    
    // Game over screen using ncurses
    nodelay(stdscr, FALSE); // Turn off non-blocking mode for final input
    clear();
    printw("%s\n", gameOverMessage);
    printw("%s%d\n", finalScoreMessage, score);
    printw("%s\n", exitMessage);
    refresh();
    getch();  // Wait for final keypress
    endwin();  // Properly cleanup ncurses
    return 0;
}

void setup()
{
    gameOver = false;
    dir = Stop;
    x = width / 2;
    y = height / 2;

    fruitx.clear();
    fruity.clear();
    for (int i = 0; i < fruitAmount; i++)
    {
        fruitx.push_back(rand() % width);
        fruity.push_back(rand() % height);
    }

    score = 0;
    tailx.clear();
    taily.clear();
    nTail = 0;
}


//void definitions
void draw()
{
    clear(); // Use ncurses clear instead of system("clear")

    for (int i = 0; i < width + 2; i++) {
        addch(borderChar); // Use ncurses addch instead of cout
    }
    addch('\n');

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                addch(borderChar);
            if (i == y && j == x)
                addch(snakeChar);
            else
            {
                bool isFruit = false;
                // Check if current position has a fruit
                for (int k = 0; k < fruitx.size(); k++)
                {
                    if (i == fruity[k] && j == fruitx[k])
                    {
                        addch(fruitChar);
                        isFruit = true;
                        break;
                    }
                }

                if (!isFruit)
                {
                    bool print = false;
                    for (int k = 0; k < nTail; k++)
                    {
                        if (tailx[k] == j && taily[k] == i)
                        {
                            addch(tailChar);
                            print = true;
                        }
                    }
                    if (!print)
                        addch(' ');
                }
            }
            if (j == width - 1)
                addch(borderChar);
        }
        addch('\n');
    }

    for (int i = 0; i < width + 2; i++)
        addch(borderChar);
    addch('\n');

    printw("%s%d\n", scoreMessage, score); // Use ncurses printw instead of cout
    if (itemOwned[0] && itemActive[0] == true) printw("S+ "); // speed boost on
    if (itemOwned[1] && itemActive[1] == true) printw("S- "); // slow motion on
    if (itemOwned[2] && itemActive[2] == true) printw("I "); // invincible on
    printw("%s\n", quitMessage);
    printw("%s\n", shopMessage);
    refresh(); // Update the screen
}

void input() 
{
    nodelay(stdscr, TRUE); // Make ncurses getch non-blocking
    int current = getch(); // Use ncurses getch (returns int, not char)

    if (current != ERR)
    { // ERR means no key was pressed
        if (current == leftInput || current == leftInput2 || current == leftInput3)
            dir = Left;
        else if (current == rightInput || current == rightInput2 || current == rightInput3)
            dir = Right;
        else if (current == upInput || current == upInput2 || current == upInput3)
            dir = Up;
        else if (current == downInput || current == downInput2 || current == downInput3)
            dir = Down;
        else if (current == shopInput || current == shopInput2)
            shopOpen = true;
        else if (current == exitInput || current == exitInput2)
            gameOver = true;
        else if (current == '1' && itemOwned[0]) {
            // Speed boost logic
            // Toggle speed boost on/off
            if (gameSpeed == 100000) {
                gameSpeed = 50000; // Faster speed
                itemActive[0] = true; // Activate speed boost
            } else {
                gameSpeed = 100000; // Reset to normal speed
                itemActive[0] = false; // Deactivate speed boost
            }
        }
        else if (current == '2' && itemOwned[1]) {
            // Slow motion logic
            // Toggle slow motion on/off
            if (gameSpeed == 100000) {
                gameSpeed = 200000; // Slower speed
                itemActive[1] = true; // Activate slow motion
            } else {
                gameSpeed = 100000; // Reset to normal speed
                itemActive[1] = false; // Deactivate slow motion
            }
        }
        else if (current == '3' && itemOwned[2]) {
            bool invincibleActive = itemActive[2];
            // Toggle invincibility
            if (!invincibleActive) {
                itemActive[2] = true; // Activate invincibility
            } else {
                itemActive[2] = false; // Deactivate invincibility
            }
        }
    
    }
}

void Logic()
{
    int prevx = tailx.size() > 0 ? tailx[0] : x;
    int prevy = taily.size() > 0 ? taily[0] : y;
    int prev2x, prev2y;

    if (nTail > 0)
    {
        tailx[0] = x;
        taily[0] = y;
        for (int i = 1; i < nTail; i++)
        {
            prev2x = tailx[i];
            prev2y = taily[i];
            tailx[i] = prevx;
            taily[i] = prevy;
            prevx = prev2x;
            prevy = prev2y;
        }
    }

    // Move the snake based on direction
    switch (dir)
    {
    case Left:
        x--;
        break;
    case Right:
        x++;
        break;
    case Up:
        y--;
        break;
    case Down:
        y++;
        break;
    default:
        break;
    }
}
void shop() {
    nodelay(stdscr, FALSE);
    while (shopOpen == true) 
    {
        clear(); // Clear the screen for shop menu

        printw("%s\n", shopMenuMessage);
        printw("%s%d\n\n", scoreMessage, score);
        
        for (int i = 0; i < numItems; i++) 
        {
            if (i == selectedItem) {
                printw("> ");
            } else {
                printw("  ");
            }

            if (itemOwned[i]) {
                printw("%s (Owned)\n", itemNames[i]);
            } else {
                printw("%s - %d points\n", itemNames[i], itemPrices[i]);
            }
        }

        printw("\n%s\n", shopMenuInstructions);
        refresh(); // Update the screen
        
        int ch = getch(); // Actually read keyboard input!
        
        if (ch == 'x' || ch == 'X') 
        {
            shopOpen = false; // Exit the shop
            nodelay(stdscr, TRUE);
            break;
        } else if (ch == 'w' || ch == 'W' || ch == KEY_UP) {
            selectedItem = (selectedItem - 1 + numItems) % numItems; // Wrap around
        } else if (ch == 's' || ch == 'S' || ch == KEY_DOWN) {
            selectedItem = (selectedItem + 1) % numItems; // Wrap around
        } else if (ch == '\n' || ch == '\r') { // Enter key
            if (!itemOwned[selectedItem] && score >= itemPrices[selectedItem]) {
                itemOwned[selectedItem] = true;
                score -= itemPrices[selectedItem];
                printw(itemPurchasedMessage, itemNames[selectedItem]);
                printw("\n");
            } else if (itemOwned[selectedItem]) {
                printw("%s\n", itemOwnedMessage);
            } else {
                printw("%s\n", itemNotEnoughPointsMessage);
            }
            refresh(); // Update the screen
            getch(); // Wait for user input before continuing
        }
    }
}