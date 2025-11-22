#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const string FILENAME = "words.txt";
const string ERROR_MESSAGE = "Error opening file.";
const string FOUND_MESSAGE = "The word is in the file.";
const string NOT_FOUND_MESSAGE = "The word is not in the file.";
const string PROMPT_MESSAGE = "Please enter a word: ";

bool isWordInFile(const string& word, const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << ERROR_MESSAGE << endl;
        return false;
    }
    string fileWord;
    while (inputFile >> fileWord) {
        if (fileWord == word) {
            return true;
        }
    }
    return false;
}

int main() {
    string word;
    cout << PROMPT_MESSAGE;
    cin >> word;
    if (isWordInFile(word, FILENAME)) {
        cout << FOUND_MESSAGE << endl;
    } else {
        cout << NOT_FOUND_MESSAGE << endl;
    }
    return 0;
}