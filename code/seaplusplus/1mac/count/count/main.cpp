#include <iostream>
#include <cctype> // For isupper, islower, isdigit

using std::cout;
using std::cin;
using std::string;
using std::endl;

const char PROMPT[] = "Please enter any characters or phrase: ";
const char RESULT[] = "Your results are: ";
const char UPPERRESULTS[] = "Uppercase letters: ";
const char LOWERRESULTS[] = "Lowercase letters: ";
const char NUMBERRESULTS[] = "Numbers: ";

void countCharacters(const string& str, int& upper, int& lower, int& digits); // countCharacters function prototype

int main() {
    string input;
    cout << PROMPT;
    getline(cin, input); //Assigns the users input to the string input

    int upperCount, lowerCount, digitCount; // Declares the integers
    countCharacters(input, upperCount, lowerCount, digitCount); // Calls the function to count characters

    cout << RESULT << endl;
    cout << UPPERRESULTS << upperCount << endl;
    cout << LOWERRESULTS << lowerCount << endl;
    cout << NUMBERRESULTS << digitCount << endl;

    return 0;
}

void countCharacters(const string& str, int& upper, int& lower, int& digits) {
    upper = lower = digits = 0; // Initialize counters
    for (char ch : str) { // Loops through all characters
        if (isupper(ch)) ++upper; // Counter for uppercase letters
        else if (islower(ch)) ++lower; // Counter for lowercase letters
        else if (isdigit(ch)) ++digits; // Counter for numbers
    }
} // countCharacters function definition
