#include <iostream>
#include <vector>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

const int width = 80;
const int height = 20;

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

//void prototypes
void setup();
void draw();
void input();
void Logic();

int main() {
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo typed characters
    keypad(stdscr, TRUE); // Enable special keys
    srand(time(0));
    setup();
    while (!gameOver) {
        draw();
        input();
        Logic();
        
        // Handle boundary wrapping
        if (x >= width)
            x = 0;
        else if (x < 0)
            x = width - 1;
        if (y >= height)
            y = 0;
        else if (y < 0)
            y = height - 1;
            
        // Check collision with tail
        for (int i = 0; i < nTail; i++) {
            if (tailx[i] == x && taily[i] == y) {
                gameOver = true;
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
        usleep(100000); // Sleep for 100 milliseconds
    }
    
    // Game over screen using ncurses
    nodelay(stdscr, FALSE); // Turn off non-blocking mode for final input
    clear();
    printw("Game Over!\n");
    printw("Final Score: %d\n", score);
    printw("Press any key to exit...");
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

    for (int i = 0; i < width + 2; i++)
        addch('#'); // Use ncurses addch instead of cout
        addch('\n');

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                addch('#');
            if (i == y && j == x)
                addch('O');
            else
            {
                bool isFruit = false;
                // Check if current position has a fruit
                for (int k = 0; k < fruitx.size(); k++)
                {
                    if (i == fruity[k] && j == fruitx[k])
                    {
                        addch('F');
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
                            addch('o');
                            print = true;
                        }
                    }
                    if (!print)
                        addch(' ');
                }
            }
            if (j == width - 1)
                addch('#');
        }
        addch('\n');
    }

    printw("Score: %d\n", score); // Use ncurses printw instead of cout
    printw("Press 'X' to quit the game.\n");
    refresh(); // Update the screen
}

void input()
{
    nodelay(stdscr, TRUE); // Make ncurses getch non-blocking
    int current = getch(); // Use ncurses getch (returns int, not char)

    if (current != ERR)
    { // ERR means no key was pressed
        if (current == 'a' || current == KEY_LEFT)
            dir = Left;
        else if (current == 'd' || current == KEY_RIGHT)
            dir = Right;
        else if (current == 'w' || current == KEY_UP)
            dir = Up;
        else if (current == 's' || current == KEY_DOWN)
            dir = Down;
        else if (current == 'x' || current == 'X')
            gameOver = true;
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