#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <limits>

using namespace std;

const char introText[] = "Welcome to the Number generator";
const char promptText[] = "Please enter the number of random numbers to generate: ";
const char minPrompt[] = "Please enter the minimum value: ";
const char maxPrompt[] = "Please enter the maximum value: ";
const char outputPrompt[] = "Enter c to output to console or f to output to file: ";
const char outputFileNamePrompt[] = "Please enter the output file name: ";

void Generator(int count, int min, int max, const string &choice);

int main()
{
    int numCount = 0;
    int minValue, maxValue;
    string inputChoice;

    cout << introText << '\n';
    cout << promptText;
    if (!(cin >> numCount) || numCount <= 0) {
        cerr << "Invalid count\n";
        return 1;
    }

    cout << minPrompt;
    if (!(cin >> minValue)) {
        cerr << "Invalid minimum\n";
        return 1;
    }

    cout << maxPrompt;
    if (!(cin >> maxValue)) {
        cerr << "Invalid maximum\n";
        return 1;
    }

    if (minValue > maxValue) {
        // swap to make a valid range
        std::swap(minValue, maxValue);
    }


    cout << outputPrompt;
    cin >> inputChoice;

    Generator(numCount, minValue, maxValue, inputChoice);

    return 0;
}

void Generator(int count, int min, int max, const string &choice)
{
    vector<int> numbers;
    numbers.reserve(count);

    // random generator for integers in [min, max]
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(min, max);

    for (int i = 0; i < count; ++i) {
        numbers.push_back(dist(gen));
    }

    if (!choice.empty() && (choice[0] == 'f' || choice[0] == 'F')) {
        cout << "Enter output file name: ";
        string filename;
        cin >> filename;
        ofstream ofs(filename);
        if (!ofs) {
            cerr << "Failed to open output.txt for writing\n";
            return;
        }
        for (int v : numbers) ofs << v << '\n';
    } else {
        for (int v : numbers) cout << v << '\n';
    }
}