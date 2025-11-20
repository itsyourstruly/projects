#include <fstream>
#include <iostream>
#include <limits>
#include <cctype>
#include <numeric>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

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

// Prototypes
void displayMenu(int &);
void selectIO(char &, char &);
int getInput(char &, double &, double &, ifstream &);
int getListInput(char &, vector<double> &, ifstream &);
double calculate(int &, char &, double &, double &, vector<double> &);
int sendOutput(char &, double &, char &, double &, double &, ofstream &);

// Lab 2 globals and prototypes
bool g_isSorted = false;
void printList(const vector<double> &list, const string &title, char outputChoice, ofstream &f);
void bubbleSort(vector<double> &arr);
void selectionSort(vector<double> &arr);
int linearSearch(const vector<double> &arr, double value);
int binarySearch(const vector<double> &arr, double value);
bool isSorted(const vector<double> &arr);

// Arithmetic functions
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
        displayMenu(choice); // show menu

        if (choice == 14)
        { // exit
            cout << endl
                 << "Thank you for using the Simple Calculator!" << endl;
            break;
        }

        // Algorithms (10-13) are handled here; other options reuse existing flow
        if (choice >= 1 && choice <= 4)
        {
            // arithmetic - get two numbers via selected I/O
            selectIO(inputChoice, outputChoice);
            if (getInput(inputChoice, num1, num2, inputFile) != 0)
            {
                // input error, try again
                continue;
            }

            result = calculate(choice, op, num1, num2, list);             // compute result
            sendOutput(outputChoice, num1, op, num2, result, outputFile); // show/save result
        }
        else if (choice >= 5 && choice <= 9)
        {
            // statistics - get list via selected I/O
            selectIO(inputChoice, outputChoice);
            list.clear();
            if (getListInput(inputChoice, list, inputFile) != 0)
            {
                // input error, try again
                continue;
            }
            // set sorted flag based on the list just read
            g_isSorted = isSorted(list);
            // clear num1/num2 (not used for stats)
            num1 = num2 = 0;

            result = calculate(choice, op, num1, num2, list);             // compute result
            sendOutput(outputChoice, num1, op, num2, result, outputFile); // show/save result
        }
        else if (choice == 10 || choice == 11)
        {
            // Sort algorithms: get I/O choices, input list, show before/after
            selectIO(inputChoice, outputChoice);
            list.clear();
            if (getListInput(inputChoice, list, inputFile) != 0)
            {
                continue;
            }

            // set sorted flag based on the list just read
            g_isSorted = isSorted(list);

            // print original
            printList(list, string("Original List"), outputChoice, outputFile);

            if (choice == 10)
            {
                bubbleSort(list);
                printList(list, string("List (Bubble Sorted)"), outputChoice, outputFile);
            }
            else
            {
                selectionSort(list);
                printList(list, string("List (Selection Sorted)"), outputChoice, outputFile);
            }
        }
        else if (choice == 12 || choice == 13)
        {
            // Search algorithms: allow keyboard/file input and console/file output
            selectIO(inputChoice, outputChoice);

            list.clear();
            if (getListInput(inputChoice, list, inputFile) != 0)
            {
                continue;
            }
            // set sorted flag based on the list just read
            g_isSorted = isSorted(list);

            double targetValue = 0;
            int index = -1;

            cout << "Enter the value to search for: ";
            if (!(cin >> targetValue))
            {
                cerr << errorMessage << " (invalid target)" << endl;
                // clear bad input
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            if (choice == 12)
            { // Linear Search
                index = linearSearch(list, targetValue);
            }
            else
            { // Binary Search - ensure sorted prerequisite
                if (!g_isSorted)
                {
                    char sortNow = 'n';
                    cout << "List is not sorted. Binary search requires a sorted list. Sort now? (y/n): ";
                    cin >> sortNow;
                    sortNow = tolower(sortNow);
                    if (sortNow == 'y')
                    {
                        // sort by selection sort to demonstrate
                        selectionSort(list);
                        cout << "List has been sorted for binary search." << endl;
                    }
                    else
                    {
                        cout << "Binary search cancelled - list must be sorted to perform binary search." << endl;
                        // skip search
                        continue;
                    }
                }
                index = binarySearch(list, targetValue);
            }

            // Report results honoring outputChoice
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

        // ask to repeat (y/n)
        do
        {
            cout << endl
                 << repeatPrompt;
            cin >> repeat;
            repeat = tolower(repeat);
        } while (repeat != 'y' && repeat != 'n');
    } while (repeat == 'y'); // Loop will be controlled by user input

    return 0;
}
// Compute result
double calculate(int &c, char &op, double &n1, double &n2, vector<double> &list)
{
    double r = 0;
    // do calculation
    switch (c)
    {
    case 1: // addition
        op = '+';
        r = add(n1, n2);
        break;
    case 2: // subtraction
        op = '-';
        r = subtract(n1, n2);
        break;
    case 3: // multiplication
        op = '*';
        r = multiply(n1, n2);
        break;
    case 4: // division
        op = '/';
        r = divide(n1, n2);
        break;
    case 5: // sum of a list
        op = 'S';
        // use the sum function defined below
        r = sum(list.data(), static_cast<int>(list.size()));
        break;
    case 6: // mean of a list
        op = 'A';
        r = mean(list.data(), static_cast<int>(list.size()));
        break;
    case 7: // median of a list
        op = 'D';
        r = median(list.data(), static_cast<int>(list.size()));
        break;
    case 8: // min of a list
        op = 'm';
        r = min(list.data(), static_cast<int>(list.size()));
        break;
    case 9: // max of a list
        op = 'X';
        r = max(list.data(), static_cast<int>(list.size()));
        break;

    default: // invalid
        cout << "\n *** Invalid choice. ***" << endl;
    }

    return r;
} // calculate

// send output to console or file
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
            return -1; // returns error if file cannot be opened
        }
        f << outLine << endl;
        f.close();
    }
    return 0;
} // sendOutput

// add two numbers
double add(double num1, double num2)
{
    return num1 + num2;
} // add

// subtract two numbers
double subtract(double num1, double num2)
{
    return num1 - num2;
} // subtract

// multiply two numbers
double multiply(double num1, double num2)
{
    return num1 * num2;
} // multiply

// divide two numbers
double divide(double num1, double num2)
{
    if (num2 == 0) // check for division by zero
    {
        cout << "cannot divide by zero" << endl;
        return 0;
    }
    return num1 / num2;
} // divide

double sum(const double list[], int size)
{
    double s = 0.0;
    for (int i = 0; i < size; ++i)
        s += list[i];
    return s;
} // sum

double mean(const double list[], int size)
{
    if (size <= 0)
        return 0.0;
    return sum(list, size) / static_cast<double>(size);
} // mean

double median(double list[], int size)
{
    if (size <= 0)
        return 0.0;
    // copy into a vector to sort
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
} // median

double min(const double list[], int size)
{
    if (size <= 0)
        return 0.0;
    double m = list[0];
    for (int i = 1; i < size; ++i)
        if (list[i] < m)
            m = list[i];
    return m;
} // min

double max(const double list[], int size)
{
    if (size <= 0)
        return 0.0;
    double m = list[0];
    for (int i = 1; i < size; ++i)
        if (list[i] > m)
            m = list[i];
    return m;
} // max

// Display menu and get a valid choice (1-14)
void displayMenu(int &c)
{
    while (true)
    {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;
        if (!(cin >> c))
        {
            // invalid input
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

// Read a list from keyboard (sentinel -1) or from a file
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
        double v;
        while (f >> v)
        {
            list.push_back(v);
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
        // Prompt until file opens
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
        // read two numbers
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
void printList(const vector<double> &list, const string &title, char outputChoice, ofstream &f)
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
        cout << outputFileNamePrompt;
        cin >> filename;
        f.open(filename);
        if (!f)
        {
            cerr << errorMessage << endl;
            return;
        }
        f << "--- " << title << " ---" << endl;
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

// Check whether a vector is sorted in non-decreasing order
bool isSorted(const vector<double> &arr)
{
    for (size_t i = 1; i < arr.size(); ++i)
    {
        if (arr[i - 1] > arr[i])
            return false;
    }
    return true;
}
