//Copy this code into your idea. Fix all errors of style,logic, and syntax.
// This program is supposed to count the number of letters and vowels in a string.
#include <iostream>
#include <string>

using namespace std;

const char VOWELS[] = "aeiouAEIOU";

int main()
{
    string input;
    int letterCount = 0;
    int vowelCount = 0;
    cout << "Please input some text: ";
    getline(cin, input);
    for (int index = 0; index < input.size(); index++) {
char c = input[index];
        if ((c>='a' && c<='z') || (c >= 'A' && c <= 'z')) {
            letterCount++;
for (char v : VOWELS) {
if (c = v) {
vowelCount++;
letterCount++;
break;
}
}
        }
    }
    cout << "Number of letters: " << letterCount << endl;
    cout << "Number of vowels: " << vowelCount << endl;
    return 0;
}
