#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 

using namespace std;

const char ERROR[] = "Unable to open file"; // Error message if the file cannot be opened
const char PROMPT[] = "Please enter multiple words separated by spaces: "; // Initial prompt
const char REPLAY[] = "Do you want to enter another set of words? (y/n): "; // Replay prompt
const char RESULT[] = "Words have been written to "; // Tells the user what file they have written to
const char comma[] = ", ";
const char CONTINUE = 'y';
const char CONTINUE2 = 'Y';

const string FILE_NAME = "example.txt"; // File name to store user inputs

int main()
{
    ofstream outputFile(FILE_NAME); // Open file
    if (!outputFile.is_open()) // Check if the file is open
    {
        cout << ERROR << endl;
        return 1; // Exit with error code
    }

    string line;
    char choice = CONTINUE; // Variable to store user's choice to continue

    while (choice == CONTINUE || choice == CONTINUE2) // Loop until the user decides to stop
    {
        cout << PROMPT;
        getline(cin, line); // Get the entire line of input

        stringstream ss(line); // Use stringstream to process the words
        string word;
        bool firstWord = true;

        while (ss >> word) // Extract words from the line
        {
            if (!firstWord) {
                outputFile << comma; // Add a comma before each word except the first
            }
            outputFile << word;
            firstWord = false;
        }

        cout << REPLAY; // Ask if the user wants to enter another set of words
        cin >> choice; // Read the user's choice
        cin.ignore(); // Clear the input buffer
    }

    outputFile.close(); // Close the file
    cout << RESULT << FILE_NAME << endl; // Tell the user where the words have been written

    return 0;
}
