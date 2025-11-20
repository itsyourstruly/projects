#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <algorithm>
#include <unistd.h> // for sleep function on Mac/Linux

using namespace std;

// Constants for hangman body parts
const string HEAD = "(\")";
const string RARM = "\\";
const string LARM = "/";
const string BODY = "|";
const string LLEG = "/";
const string RLEG = " \\";

// Function prototypes
void drawBody(int numWrong);
void clearScreen();
char toUpperCase(char c);
bool isValidGuess(char guess, const vector<char>& guessedLetters);
string displayWord(const string& word, const vector<char>& guessedLetters);
vector<string> loadWordsFromFile(const string& filename);
string selectRandomWord(const vector<string>& words);
bool hasWonGame(const string& word, const vector<char>& guessedLetters);
void displayGuessedLetters(const vector<char>& correctGuesses, const vector<char>& wrongGuesses);
bool playAgain();

int main() {
    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Load words from file
    vector<string> words = loadWordsFromFile("words.txt");
    if (words.empty()) {
        cout << "Error: Could not load words from file 'words.txt'" << endl;
        return 1;
    }
    
    // Welcome message
    cout << "==================================" << endl;
    cout << "    WELCOME TO HANGMAN GAME!" << endl;
    cout << "==================================" << endl;
    cout << "Press Enter to start...";
    cin.get();
    
    // Main game loop
    bool keepPlaying = true;
    while (keepPlaying) {
        // Initialize game variables for new round
        string secretWord = selectRandomWord(words);
        vector<char> guessedLetters;
        vector<char> correctGuesses;
        vector<char> wrongGuesses;
        int numWrong = 0;
        const int MAX_WRONG = 6;
        bool gameWon = false;
        bool gameLost = false;
        
        // Game round loop
        while (!gameWon && !gameLost) {
            clearScreen();
            
            // Draw hangman
            drawBody(numWrong);
            cout << endl;
            
            // Display current word state
            cout << "Word: " << displayWord(secretWord, guessedLetters) << endl;
            cout << endl;
            
            // Display guessed letters
            displayGuessedLetters(correctGuesses, wrongGuesses);
            cout << endl;
            
            // Get player's guess
            char guess;
            bool validInput = false;
            while (!validInput) {
                cout << "Enter your guess (a letter): ";
                cin >> guess;
                guess = toUpperCase(guess);
                
                if (isValidGuess(guess, guessedLetters)) {
                    validInput = true;
                } else {
                    cout << "Invalid guess! Please enter a letter you haven't guessed before." << endl;
                }
            }
            
            // Add guess to guessed letters
            guessedLetters.push_back(guess);
            
            // Check if guess is correct
            bool correctGuess = false;
            for (char c : secretWord) {
                if (toUpperCase(c) == guess) {
                    correctGuess = true;
                    break;
                }
            }
            
            if (correctGuess) {
                correctGuesses.push_back(guess);
                cout << "Good guess!" << endl;
            } else {
                wrongGuesses.push_back(guess);
                numWrong++;
                cout << "Sorry, that letter is not in the word." << endl;
            }
            
            // Check win/lose conditions
            gameWon = hasWonGame(secretWord, guessedLetters);
            gameLost = (numWrong >= MAX_WRONG);
            
            if (!gameWon && !gameLost) {
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
            }
        }
        
        // End of round - show final state
        clearScreen();
        drawBody(numWrong);
        cout << endl;
        
        if (gameWon) {
            cout << "🎉 CONGRATULATIONS! 🎉" << endl;
            cout << "You guessed the word: " << secretWord << endl;
        } else {
            cout << "💀 GAME OVER! 💀" << endl;
            cout << "The word was: " << secretWord << endl;
        }
        
        cout << endl;
        keepPlaying = playAgain();
    }
    
    cout << "Thanks for playing Hangman! Goodbye!" << endl;
    return 0;
}

void drawBody(int numWrong) {
    string body[6] = { HEAD, LARM, BODY, RARM, LLEG, RLEG };
    string score[6] = { "", "", "", "", "", "" };
    
    for (int counter = 0; counter < numWrong && counter < 6; counter++) {
        score[counter] = body[counter];
    }
    
    string image = "########\n#      " + score[0] + "\n#      " + score[1] + score[2] + score[3];
    image += "\n#      " + score[4] + score[5] + "\n#\n################\n";
    cout << image;
}

void clearScreen() {
    system("clear"); // For Mac/Linux
}

char toUpperCase(char c) {
    return static_cast<char>(toupper(c));
}

bool isValidGuess(char guess, const vector<char>& guessedLetters) {
    // Check if it's a letter
    if (!isalpha(guess)) {
        return false;
    }
    
    // Check if already guessed
    for (char c : guessedLetters) {
        if (c == guess) {
            return false;
        }
    }
    
    return true;
}

string displayWord(const string& word, const vector<char>& guessedLetters) {
    string display = "";
    for (char c : word) {
        bool found = false;
        for (char guess : guessedLetters) {
            if (toUpperCase(c) == guess) {
                found = true;
                break;
            }
        }
        
        if (found) {
            display += c;
        } else {
            display += "_";
        }
    }
    return display;
}

vector<string> loadWordsFromFile(const string& filename) {
    vector<string> words;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cout << "Error: Cannot open file " << filename << endl;
        return words;
    }
    
    string word;
    while (getline(file, word)) {
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    
    file.close();
    return words;
}

string selectRandomWord(const vector<string>& words) {
    if (words.empty()) {
        return "";
    }
    
    int randomIndex = rand() % words.size();
    return words[randomIndex];
}

bool hasWonGame(const string& word, const vector<char>& guessedLetters) {
    for (char c : word) {
        bool found = false;
        for (char guess : guessedLetters) {
            if (toUpperCase(c) == guess) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

void displayGuessedLetters(const vector<char>& correctGuesses, const vector<char>& wrongGuesses) {
    if (!correctGuesses.empty()) {
        cout << "Correct guesses: ";
        for (size_t i = 0; i < correctGuesses.size(); i++) {
            cout << correctGuesses[i];
            if (i < correctGuesses.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    
    if (!wrongGuesses.empty()) {
        cout << "Wrong guesses: ";
        for (size_t i = 0; i < wrongGuesses.size(); i++) {
            cout << wrongGuesses[i];
            if (i < wrongGuesses.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

bool playAgain() {
    char choice;
    cout << "Would you like to play again? (y/n): ";
    cin >> choice;
    choice = tolower(choice);
    return (choice == 'y');
}
