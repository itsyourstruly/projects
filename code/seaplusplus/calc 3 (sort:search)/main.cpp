#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

// Menu and prompt strings
const char menuTitle[] = "Calculator Menu";
const char menuOptions[] = "1. Add\n2. Subtract\n3. Multiply\n4. Divide\n5. Sum\n6. Mean\n7. Median\n8. Min\n9. Max\n--- Algorithms ---\n10. Bubble Sort\n11. Selection Sort\n12. Linear Search\n13. Binary Search\n--- System ---\n14. Exit\n";
const char menuPrompt[] = "Please input an option from the above menu (1-14)";
const char repeatPrompt[] = "Would you like to perform another calculation? (Y/N): ";
const char inputMethods[] = "Select input method - keyboard (k) or file (f): ";
const char outputMethods[] = "Select output method - console (c) or file (f): ";
const char firstNumberPrompt[] = "Enter first number: ";
const char secondNumberPrompt[] = "Enter second number: ";
const char inputFileNamePrompt[] = "Enter input file name: ";
const char outputFileNamePrompt[] = "Enter output file name: ";
const char errorMessage[] = "Error, please try again.";

// Main menu and I/O functions
void displayMenu(int &);
void selectIO(char &, char &);
int getInput(char &, double &, double &, ifstream &);
int getListInput(char &, vector<double> &, ifstream &);
double calculate(int &, char &, double &, double &, vector<double> &);
int sendOutput(char &, double &, char &, double &, double &, ofstream &);

// Sorting and searching functions
bool g_isSorted = false;
void printList(const vector<double> &list, const string &title, char outputChoice, ofstream &f, const string &filePrompt = string(outputFileNamePrompt));
void bubbleSort(vector<double> &arr);
void selectionSort(vector<double> &arr);
int linearSearch(const vector<double> &arr, double value);
int binarySearch(const vector<double> &arr, double value);
bool isSorted(const vector<double> &arr);

// Math functions
double add(double, double);
double subtract(double, double);
double multiply(double, double);
double divide(double, double);
double sum(const double list[], int size);
double mean(const double list[], int size);
double median(double list[], int size);
double min(const double list[], int size);
double max(const double list[], int size);

int main()
{
    int choice = 0;
    double num1 = 0, num2 = 0, result = 0;
    char inputChoice = '\0', outputChoice = '\0', op = '\0', repeat = 'y';
    vector<double> list = {};
    ifstream inputFile;
    ofstream outputFile;

    do
    {
        displayMenu(choice);

        if (choice == 14)
        {
            cout << endl
                 << "Thank you for using the Simple Calculator!" << endl;
            break;
        }

        // Handle basic arithmetic (add, subtract, multiply, divide)
        if (choice >= 1 && choice <= 4)
        {
            selectIO(inputChoice, outputChoice);
            if (getInput(inputChoice, num1, num2, inputFile) != 0)
            {
                continue;
            }

            result = calculate(choice, op, num1, num2, list);
            sendOutput(outputChoice, num1, op, num2, result, outputFile);
        }
        // Handle statistics (sum, mean, median, min, max)
        else if (choice >= 5 && choice <= 9)
        {
            selectIO(inputChoice, outputChoice);
            list.clear();
            if (getListInput(inputChoice, list, inputFile) != 0)
            {
                continue;
            }
            g_isSorted = isSorted(list);
            num1 = num2 = 0;

            result = calculate(choice, op, num1, num2, list);
            sendOutput(outputChoice, num1, op, num2, result, outputFile);
        }
        // Handle sorting (bubble sort, selection sort)
        else if (choice == 10 || choice == 11)
        {
            selectIO(inputChoice, outputChoice);
            list.clear();
            if (getListInput(inputChoice, list, inputFile) != 0)
            {
                continue;
            }

            g_isSorted = isSorted(list);

            // Show the unsorted list
            printList(list, string("Original List"), outputChoice, outputFile, "Unsorted file name: ");

            if (choice == 10)
            {
                bubbleSort(list);
                printList(list, string("List (Bubble Sorted)"), outputChoice, outputFile, "Sorted file name: ");
            }
            else
            {
                selectionSort(list);
                printList(list, string("List (Selection Sorted)"), outputChoice, outputFile, "Sorted file name: ");
            }
        }
        // Handle searching (linear search, binary search)
        else if (choice == 12 || choice == 13)
        {
            selectIO(inputChoice, outputChoice);

            list.clear();
            if (getListInput(inputChoice, list, inputFile) != 0)
            {
                continue;
            }
            g_isSorted = isSorted(list);

            double targetValue = 0;
            int index = -1;

            cout << "Enter the value to search for: ";
            if (!(cin >> targetValue))
            {
                cerr << errorMessage << " (invalid target)" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            // Perform linear search
            if (choice == 12)
            {
                index = linearSearch(list, targetValue);
            }
            // Perform binary search (requires sorted list)
            else
            {
                if (!g_isSorted)
                {
                    char sortNow = 'n';
                    cout << "List is not sorted. Binary search requires a sorted list. Sort now? (y/n): ";
                    cin >> sortNow;
                    sortNow = tolower(sortNow);
                    if (sortNow == 'y')
                    {
                        selectionSort(list);
                        cout << "List has been sorted for binary search." << endl;
                    }
                    else
                    {
                        cout << "Binary search cancelled - list must be sorted to perform binary search." << endl;
                        continue;
                    }
                }
                index = binarySearch(list, targetValue);
            }

            // Display search results
            string resultLine;
            if (index != -1)
            {
                resultLine = string("Value ") + to_string(targetValue) + string(" was FOUND at index ") + to_string(index) + string(".");
            }
            else
            {
                resultLine = string("Value ") + to_string(targetValue) + string(" was NOT FOUND.");
            }

            if (outputChoice == 'c')
            {
                cout << "\n--- Result ---" << endl;
                cout << resultLine << endl;
            }
            else if (outputChoice == 'f')
            {
                string filename;
                cout << outputFileNamePrompt;
                cin >> filename;
                outputFile.open(filename);
                if (!outputFile)
                {
                    cerr << errorMessage << " (could not open output file)" << endl;
                }
                else
                {
                    outputFile << "--- Result ---" << endl;
                    outputFile << resultLine << endl;
                    outputFile.close();
                }
            }
        }
        else
        {
            cout << "\n *** Invalid choice. ***" << endl;
            continue;
        }

        // Ask user if they want to repeat
        do
        {
            cout << endl
                 << repeatPrompt;
            cin >> repeat;
            repeat = tolower(repeat);
        } while (repeat != 'y' && repeat != 'n');
    } while (repeat == 'y');

    return 0;
}

// Perform the calculation based on user's choice
double calculate(int &c, char &op, double &n1, double &n2, vector<double> &list)
{
    double r = 0;
    switch (c)
    {
    case 1:
        op = '+';
        r = add(n1, n2);
        break;
    case 2:
        op = '-';
        r = subtract(n1, n2);
        break;
    case 3:
        op = '*';
        r = multiply(n1, n2);
        break;
    case 4:
        op = '/';
        r = divide(n1, n2);
        break;
    case 5:
        op = 'S';
        r = sum(list.data(), static_cast<int>(list.size()));
        break;
    case 6:
        op = 'A';
        r = mean(list.data(), static_cast<int>(list.size()));
        break;
    case 7:
        op = 'D';
        r = median(list.data(), static_cast<int>(list.size()));
        break;
    case 8:
        op = 'm';
        r = min(list.data(), static_cast<int>(list.size()));
        break;
    case 9:
        op = 'X';
        r = max(list.data(), static_cast<int>(list.size()));
        break;

    default:
        cout << "\n *** Invalid choice. ***" << endl;
    }

    return r;
}

// Output the result to console or file
int sendOutput(char &o, double &n1, char &op, double &n2, double &r, ofstream &f)
{
    string outLine;
    if (op == '+' || op == '-' || op == '*' || op == '/')
    {
        outLine = to_string(n1) + " " + op + " " + to_string(n2) + " = " + to_string(r);
    }
    else
    {
        string label;
        switch (op)
        {
        case 'S':
            label = "Sum";
            break;
        case 'A':
            label = "Mean";
            break;
        case 'D':
            label = "Median";
            break;
        case 'm':
            label = "Min";
            break;
        case 'X':
            label = "Max";
            break;
        default:
            label = "Result";
        }
        outLine = label + " = " + to_string(r);
    }

    if (o == 'c')
    {
        cout << outLine << endl;
    }
    else if (o == 'f')
    {
        string filename;
        cout << outputFileNamePrompt;
        cin >> filename;
        f.open(filename);
        if (!f)
        {
            cerr << errorMessage << endl;
            return -1;
        }
        f << outLine << endl;
        f.close();
    }
    return 0;
}

// Add two numbers
double add(double num1, double num2)
{
    return num1 + num2;
}

// Subtract two numbers
double subtract(double num1, double num2)
{
    return num1 - num2;
}

// Multiply two numbers
double multiply(double num1, double num2)
{
    return num1 * num2;
}

// Divide two numbers
double divide(double num1, double num2)
{
    if (num2 == 0)
    {
        cout << "cannot divide by zero" << endl;
        return 0;
    }
    return num1 / num2;
}

// Calculate the sum of numbers in a list
double sum(const double list[], int size)
{
    double s = 0.0;
    for (int i = 0; i < size; ++i)
        s += list[i];
    return s;
}

// Calculate the average of numbers in a list
double mean(const double list[], int size)
{
    if (size <= 0)
        return 0.0;
    return sum(list, size) / static_cast<double>(size);
}

// Find the middle value in a list
double median(double list[], int size)
{
    if (size <= 0)
        return 0.0;
    vector<double> tmp(list, list + size);
    sort(tmp.begin(), tmp.end());
    if (size % 2 == 1)
    {
        return tmp[size / 2];
    }
    else
    {
        double a = tmp[size / 2 - 1];
        double b = tmp[size / 2];
        return (a + b) / 2.0;
    }
}

// Find the smallest number in a list
double min(const double list[], int size)
{
    if (size <= 0)
        return 0.0;
    double m = list[0];
    for (int i = 1; i < size; ++i)
        if (list[i] < m)
            m = list[i];
    return m;
}

// Find the largest number in a list
double max(const double list[], int size)
{
    if (size <= 0)
        return 0.0;
    double m = list[0];
    for (int i = 1; i < size; ++i)
        if (list[i] > m)
            m = list[i];
    return m;
}

// Display menu and get user's choice
void displayMenu(int &c)
{
    while (true)
    {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;
        if (!(cin >> c))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 14." << endl;
            continue;
        }
        if (c >= 1 && c <= 14)
            break;
        cout << "Choice out of range. Try again." << endl;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Read a list from keyboard or file
int getListInput(char &i, vector<double> &list, ifstream &f)
{
    if (i == 'k')
    {
        cout << "Enter numbers one at a time. Enter -1 to finish (sentinel)." << endl;
        double v = 0;
        while (true)
        {
            cout << "Number: ";
            if (!(cin >> v))
            {
                cerr << errorMessage << " (invalid number)" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            if (v == -1)
                break;
            list.push_back(v);
        }
        if (list.empty())
        {
            cerr << "No numbers entered." << endl;
            return -1;
        }
    }
    else if (i == 'f')
    {
        string filename;
        while (true)
        {
            cout << inputFileNamePrompt;
            cin >> filename;
            f.clear();
            f.open(filename);
            if (!f.is_open())
            {
                cerr << errorMessage << " (could not open file)" << endl;
            }
            else
                break;
        }
        // Read numbers from file (supports plain numbers or "index: value" format)
        string line;
        while (getline(f, line))
        {
            if (line.empty())
                continue;
            size_t colonPos = line.find(':');
            string toParse = (colonPos != string::npos) ? line.substr(colonPos + 1) : line;

            istringstream ss(toParse);
            double value;
            while (ss >> value)
            {
                list.push_back(value);
            }
        }
        if (list.empty())
        {
            cerr << "No numbers found in file." << endl;
            f.close();
            return -1;
        }
        f.close();
    }
    else
    {
        cerr << errorMessage << endl;
        return -1;
    }
    return 0;
}

// Choose input (k/f) and output (c/f)
void selectIO(char &i, char &o)
{
    while (i != 'k' && i != 'f')
    {
        cout << inputMethods;
        cin >> i;
        i = tolower(i);
    }
    while (o != 'c' && o != 'f')
    {
        cout << outputMethods;
        cin >> o;
        o = tolower(o);
    }
}

// Read two numbers from keyboard or file
int getInput(char &i, double &n1, double &n2, ifstream &f)
{
    if (i == 'k')
    {
        cout << firstNumberPrompt;
        if (!(cin >> n1)) return -1;
        cout << secondNumberPrompt;
        if (!(cin >> n2)) return -1;
    }
    else if (i == 'f')
    {
        string filename;
        while (true)
        {
            cout << inputFileNamePrompt;
            cin >> filename;
            f.clear();
            f.open(filename);
            if (!f.is_open())
            {
                cerr << errorMessage << endl;
            }
            else
            {
                break;
            }
        }
        if (!(f >> n1 >> n2))
        {
            cerr << errorMessage << " (failed to read two numbers)" << endl;
            f.close();
            return -1;
        }
        f.close();
    }
    else
    {
        cerr << errorMessage << endl;
        return -1;
    }
    return 0;
}

// Print list to console or file
void printList(const vector<double> &list, const string &title, char outputChoice, ofstream &f, const string &filePrompt)
{
    if (outputChoice == 'c')
    {
        cout << "\n--- " << title << " ---" << endl;
        for (size_t i = 0; i < list.size(); ++i)
        {
            cout << i << ": " << list[i] << endl;
        }
    }
    else if (outputChoice == 'f')
    {
        string filename;
        cout << filePrompt;
        cin >> filename;
        f.open(filename);
        if (!f)
        {
            cerr << errorMessage << endl;
            return;
        }
        for (size_t i = 0; i < list.size(); ++i)
        {
            f << i << ": " << list[i] << endl;
        }
        f.close();
    }
}

// Bubble sort (ascending)
void bubbleSort(vector<double> &arr)
{
    int n = static_cast<int>(arr.size());
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
    g_isSorted = true;
}

// Selection sort (ascending)
void selectionSort(vector<double> &arr)
{
    int n = static_cast<int>(arr.size());
    for (int i = 0; i < n - 1; ++i)
    {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j)
            if (arr[j] < arr[minIdx])
                minIdx = j;
        if (minIdx != i)
            swap(arr[i], arr[minIdx]);
    }
    g_isSorted = true;
}

// Linear search - returns index or -1
int linearSearch(const vector<double> &arr, double value)
{
    for (size_t i = 0; i < arr.size(); ++i)
        if (arr[i] == value)
            return static_cast<int>(i);
    return -1;
}

// Binary search assumes arr is sorted ascending - returns index or -1
int binarySearch(const vector<double> &arr, double value)
{
    int left = 0;
    int right = static_cast<int>(arr.size()) - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (arr[mid] == value)
            return mid;
        else if (arr[mid] < value)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// Check whether a vector is sorted in ascending order
bool isSorted(const vector<double> &arr)
{
    for (size_t i = 1; i < arr.size(); ++i)
    {
        if (arr[i - 1] > arr[i])
            return false;
    }
    return true;
}
