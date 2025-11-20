#include <iostream> // Required for input/output operations (cin, cout)
#include <string>   // Required for using the string data type
#include <limits>   // Required for numeric_limits to clear input buffer

using namespace std;

//prototypes
bool isLetter(char value);
bool isNum(char value);
bool isLetter(string value);
bool isNum(string value);

const char PROMPT[] = "Please enter some text: ";
const char RESULTS[] = "Character Counts";
const char CAPITALRESULTS[] = "Number of Capital letters: ";
const char LOWERRESULTS[] = "Number of Lowercase letters: ";
const char NUMBERRESULTS[] = "Number of Numbers: ";

int main() {
    string Input;
    int capitalLetterCount = 0;
    int lowerCaseLetterCount = 0;
    int numberCount = 0;

    cout << PROMPT;
    getline(cin, Input);

    for (char c : Input) 
    {
        if (c >= 'A' && c <= 'Z') 
        {
            capitalLetterCount++;
        }
        else if (c >= 'a' && c <= 'z') 
        {
            lowerCaseLetterCount++;
        }
        else if (c >= '0' && c <= '9') 
        {
            numberCount++;
        }
    }

// Results
    cout << RESULTS << endl;
    cout << CAPITALRESULTS << capitalLetterCount << endl;
    cout << LOWERRESULTS << lowerCaseLetterCount << endl;
    cout << NUMBERRESULTS << numberCount << endl;

    // --- Testing the Overloaded Functions ---
    cout << "\n--- Testing isLetter(string) and isNum(string) ---" << endl;

    // Inform the user how to exit the program (if running from a console)
    cout << "\nPress Enter to exit the program." << endl;
    // Clear the input buffer before waiting for the final Enter
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get(); // Wait for user to press Enter

    return 0; // Indicate successful program execution
}


// --- Function Definitions ---

// Definition for isLetter(char value)
bool isLetter(char value) {
    // Check if the character is within the ASCII range for capital letters (A-Z)
    // OR if it's within the ASCII range for lowercase letters (a-z).
    // The comparison works directly with char types as they implicitly convert to their ASCII values.
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
}

// Definition for isNum(char value)
bool isNum(char value) {
    // Check if the character is within the ASCII range for digits (0-9).
    return (value >= '0' && value <= '9');
}

// Definition for isLetter(string value) - Overloaded version
bool isLetter(string value) {
    // If the string is empty, it technically contains no non-letters, so return true.
    if (value.empty()) {
        return true;
    }
    // Iterate through each character in the string.
    // If any character is NOT a letter (using our isLetter(char) function),
    // then the entire string is not composed solely of letters.
    for (char c : value) {
        if (!isLetter(c)) { // Call the single-character version of isLetter
            return false; // Found a non-letter, so return false immediately
        }
    }
    // If the loop completes, it means all characters were letters.
    return true;
}

// Definition for isNum(string value) - Overloaded version
bool isNum(string value) {
    // If the string is empty, it technically contains no non-numbers, so return true.
    if (value.empty()) {
        return true;
    }
    // Iterate through each character in the string.
    // If any character is NOT a number (using isNum(char) function),
    // then the entire string is not made solely of numbers.
    for (char c : value) {
        if (!isNum(c)) { // Call the single-character version of isNum
            return false; // Found a non-number, so return false immediately
        }
    }
    // If the loop completes, it means all characters were numbers.
    return true;
}
