#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <cctype>
#include <unistd.h> // for sleep function on Mac/Linux

using namespace std;

// Constants for hangman drawing
const string HEAD = "(\")";
const string RightARM = "\\";
const string LeftARM = "/";
const string BODY = "|";
const string LeftLEG = "/";
const string RightLEG = " \\";

// Game constants
const string wordsFile = "words.txt";
const string fileOpenError = "Error: Cannot open words.txt file!";
const string noWordsError = "Error: No words found in file!";
const string gameTitle = "=== HANGMAN GAME ===";
const string gameOverTitle = "=== GAME OVER ===";
const string wordLabel = "Word: ";
const string underscore = "_";
const string wrongLettersLabel = "Wrong letters: ";
const string space = " ";
const string guessPrompt = "Enter your guess: ";
const string invalidLetterMsg = "Please enter a letter!";
const string alreadyGuessedMsg = "You already guessed that letter!";
const string goodGuessMsg = "Good guess!";
const string badGuessPrefix = "Sorry, '";
const string badGuessSuffix = "' is not in the word.";
const string winMsg = "Congratulations! You won!";
const string loseMsg = "You lost! The hangman is complete.";
const string wordRevealPrefix = "The word was: ";
const string playAgainPrompt = "Do you want to play again? (y/n): ";
const string thanksMsg = "Thanks for playing!";
const string clearScreenAnsi = "\033[2J\033[1;1H";
const string hangmanFrame = "########\n#      ";
const string hangmanMiddle = "\n#      ";
const string hangmanBottom = "\n#\n################\n";
const string emptyString = "";

// Function prototypes
void drawBody(int numwrong);
void clearScreen();
char toUpper(char letter);
bool isValidGuess(char guess, vector<char>& guessedLetters);
void showWord(string word, vector<char>& correctGuesses);
bool loadWords(vector<string>& words);
string getRandomWord(vector<string>& words);
bool processGuess(char guess, string currentWord, vector<char>& correctGuesses, vector<char>& wrongGuesses, int& numWrong);
bool isWordComplete(string currentWord, vector<char>& correctGuesses);
void showGameOver(bool gameWon, int numWrong, string currentWord);

int main() {
    vector<string> words;
    
    if (!loadWords(words)) {
        return 1;
    }
    
    char playAgain = 'y';
    
    while (playAgain == 'y' || playAgain == 'Y') {
        string currentWord = getRandomWord(words);
        
        vector<char> correctGuesses;
        vector<char> wrongGuesses;
        vector<char> allGuesses;
        int numWrong = 0;
        bool gameWon = false;
        
        while (numWrong < 6 && !gameWon) {
            clearScreen();
            
            cout << gameTitle << endl << endl;
            drawBody(numWrong);
            cout << endl;
            showWord(currentWord, correctGuesses);
            cout << endl;
            
            if (!wrongGuesses.empty()) {
                cout << wrongLettersLabel;
                for (char letter : wrongGuesses) {
                    cout << letter << space;
                }
                cout << endl;
            }
            cout << endl;
            
            char guess;
            cout << guessPrompt;
            cin >> guess;
            
            if (!isValidGuess(guess, allGuesses)) {
                if (!isalpha(guess)) {
                    cout << invalidLetterMsg << endl;
                } else {
                    cout << alreadyGuessedMsg << endl;
                }
                sleep(1);
                continue;
            }
            
            guess = toUpper(guess);
            allGuesses.push_back(guess);
            
            processGuess(guess, currentWord, correctGuesses, wrongGuesses, numWrong);
            gameWon = isWordComplete(currentWord, correctGuesses);
            
            sleep(1);
        }
        
        showGameOver(gameWon, numWrong, currentWord);
        
        cout << playAgainPrompt;
        cin >> playAgain;
    }
    
    cout << thanksMsg << endl;
    return 0;
}

// Function definitions

// Function to draw the hangman
void drawBody(int numwrong) {
    string body[6] = { HEAD, LeftARM, BODY, RightARM, LeftLEG, RightLEG }; // array of body parts
    string score[6] = { emptyString, emptyString, emptyString, emptyString, emptyString, emptyString }; // an array to hold body parts
    
    for (int counter = 0; counter <= numwrong; counter++) { // draw body parts based on numwrong
        score[counter] = body[counter];
    }
    
    string image = hangmanFrame + score[0] + hangmanMiddle + score[1] + score[2] + score[3];
    image += hangmanMiddle + score[4] + score[5] + hangmanBottom;
    cout << image;
}

// Function to clear screen
void clearScreen() {
    cout << clearScreenAnsi; // ANSI escape codes for Mac
}

// Function to convert character to uppercase
char toUpper(char letter) {
    return toupper(letter);
}

// Function to check if guess is valid
bool isValidGuess(char guess, vector<char>& guessedLetters) {
    if (!isalpha(guess)) {
        return false;
    }
    
    char upperGuess = toUpper(guess);
    for (char letter : guessedLetters) {
        if (letter == upperGuess) {
            return false; // already guessed
        }
    }
    return true;
}

// Function to display the word with blanks
void showWord(string word, vector<char>& correctGuesses) {
    cout << wordLabel;
    for (char letter : word) {
        bool found = false;
        for (char guess : correctGuesses) {
            if (letter == guess) {
                cout << letter;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << underscore;
        }
    }
    cout << endl;
}

// Function to load words from file
bool loadWords(vector<string>& words) {
    ifstream file(wordsFile);
    
    if (!file.is_open()) {
        cout << fileOpenError << endl;
        return false;
    }
    
    string word;
    while (getline(file, word)) {
        if (!word.empty()) {
            transform(word.begin(), word.end(), word.begin(), ::toupper);
            words.push_back(word);
        }
    }
    file.close();
    
    if (words.empty()) {
        cout << noWordsError << endl;
        return false;
    }
    
    return true;
}

// Function to get random word
string getRandomWord(vector<string>& words) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, words.size() - 1);
    return words[dis(gen)];
}

// Function to process a guess
bool processGuess(char guess, string currentWord, vector<char>& correctGuesses, vector<char>& wrongGuesses, int& numWrong) {
    bool correctGuess = false;
    for (char letter : currentWord) {
        if (letter == guess) {
            correctGuess = true;
            break;
        }
    }
    
    if (correctGuess) {
        correctGuesses.push_back(guess);
        cout << goodGuessMsg << endl;
    } else {
        wrongGuesses.push_back(guess);
        numWrong++;
        cout << badGuessPrefix << guess << badGuessSuffix << endl;
    }
    
    return correctGuess;
}

// Function to check if word is complete
bool isWordComplete(string currentWord, vector<char>& correctGuesses) {
    for (char letter : currentWord) {
        bool found = false;
        for (char correctGuess : correctGuesses) {
            if (letter == correctGuess) {
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

// Function to show game over screen
void showGameOver(bool gameWon, int numWrong, string currentWord) {
    clearScreen();
    cout << gameOverTitle << endl << endl;
    drawBody(numWrong);
    cout << endl;
    
    if (gameWon) {
        cout << winMsg << endl;
    } else {
        cout << loseMsg << endl;
    }
    cout << wordRevealPrefix << currentWord << endl << endl;
}
