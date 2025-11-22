// Pseudo code is a simplified, informal way of describing a program's logic and flow without using the syntax of a specific programming language. It uses plain language and basic programming constructs to outline the steps needed to solve a problem or perform a task. Pseudo code helps programmers plan and communicate their ideas before writing actual code. It focuses on the logic rather than the syntax, making it easier to understand and modify.
// Here's an example of pseudo code for a program that asks the user to input text and then makes that text scroll across the screen from left to right:
// 1.    Start
// 2.    Prompt the user to enter some text
// 3.    Read the user's input
// 4.    Define the width of the display window
// 5.    Define the delay between each scroll step
// 6.    Create a string with spaces added to the beginning and end of the input text
// 7.    Loop through the string, displaying a substring of the defined width
// 8.    Pause for the defined delay
// 9.    Repeat the loop until the entire string has been displayed
// 10.    End
// Pseudo code helps in visualizing the program's structure and logic, making it easier to translate into actual code in any programming language.

// Does this code match the pseudocode?

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// Constants
const int DISPLAY_WIDTH = 50;
const int SCROLL_DELAY = 200;

void scrollText(const string& text, int width, int delay) {
    string displayText = string(width, ' ') + text + string(width, ' ');
    int textLength = displayText.length();

    for (int i = 0; i <= textLength - width; ++i) {
        cout << "\r" << displayText.substr(i, width) << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
}

int main() {
    string inputText;

    cout << "Please enter some text: ";
    getline(cin, inputText);

    scrollText(inputText, DISPLAY_WIDTH, SCROLL_DELAY);

    return 0;
}