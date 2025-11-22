#include <iostream>
#include <string>

using namespace std;

const char PROMPT[] = "Please input some text: "; // Prompt for user input
const char VOWELS[] = "aeiouAEIOU"; // string containing all vowels
const char LETTERSCOUNT[] = "Number of letters: "; // String to display letter count
const char VOWELSCOUNT[] = "Number of vowels: "; // String to display vowel count

int main()
{
    string input;
    int letterCount = 0;
    int vowelCount = 0;

    cout << PROMPT;
	getline(cin, input); // Read a line of input from the user
    for (size_t index = 0; index < input.size(); index++) {
        char c = input[index];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { // Check if character is a letter
            letterCount++;
			for (char v : VOWELS) { // Check if character is a vowel
                if (c == v) {
                    vowelCount++;
                    break;
                }
            }
        }
    }
	cout << LETTERSCOUNT << letterCount << endl; // Display letter count
	cout << VOWELSCOUNT << vowelCount << endl; // Display vowel count
    return 0;
}
