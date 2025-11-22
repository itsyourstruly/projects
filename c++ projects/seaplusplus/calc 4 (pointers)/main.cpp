#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <algorithm>
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
const int MAX_SIZE = 100; // Maximum size for dynamic arrays

// Main menu and I/O functions - REFACTORED TO USE POINTERS
void displayMenu(int* choicePtr);
void selectIO(char* inputChoicePtr, char* outputChoicePtr);
int getInput(char* inputChoicePtr, double* n1Ptr, double* n2Ptr, ifstream* f);
int getListInput(char* inputChoicePtr, double* list, int maxSize, ifstream* f);
double calculate(int* choicePtr, char* opPtr, double* n1Ptr, double* n2Ptr, double* listPtr, int size);
int sendOutput(char* outputChoicePtr, double* n1Ptr, char* opPtr, double* n2Ptr, double* resultPtr, ofstream* f);

// Sorting and searching functions - REFACTORED TO USE POINTERS
bool g_isSorted = false;
void printList(const double* list, int size, const string& title, char* outputChoicePtr, ofstream* f, const string& filePrompt = string(outputFileNamePrompt));
void bubbleSort(double* arr, int size);
void selectionSort(double* arr, int size);
int linearSearch(const double* arr, int size, double value);
int binarySearch(const double* arr, int size, double value);
bool isSorted(const double* arr, int size);

// Math functions - REFACTORED TO USE POINTERS
double add(double, double);
double subtract(double, double);
double multiply(double, double);
double divide(double, double);
double sum(const double* list, int size);
double mean(const double* list, int size);
double median(double* list, int size);
double min(const double* list, int size);
double max(const double* list, int size);

int main()
{
    int choice = 0;
    double num1 = 0, num2 = 0, result = 0;
    char inputChoice = '\0', outputChoice = '\0', op = '\0', repeat = 'y';
    // DYNAMIC MEMORY: Pointer for dynamically allocated array
    double* listPtr = nullptr;
    int count = 0;
    ifstream inputFile;
    ofstream outputFile;

    do
    {
        // POINTER USAGE: Pass address of choice
        displayMenu(&choice);

        if (choice == 14)
        {
            cout << endl
                 << "Thank you for using the Simple Calculator!" << endl;
            break;
        }

        // Handle basic arithmetic (add, subtract, multiply, divide)
        if (choice >= 1 && choice <= 4)
        {
            // POINTER USAGE: Pass addresses of variables
            selectIO(&inputChoice, &outputChoice);
            if (getInput(&inputChoice, &num1, &num2, &inputFile) != 0)
            {
                continue;
            }

            result = calculate(&choice, &op, &num1, &num2, nullptr, 0);
            sendOutput(&outputChoice, &num1, &op, &num2, &result, &outputFile);
        }
        // Handle statistics (sum, mean, median, min, max)
        else if (choice >= 5 && choice <= 9)
        {
            // POINTER USAGE: Pass addresses of variables
            selectIO(&inputChoice, &outputChoice);

            // DYNAMIC MEMORY: Allocate memory for list
            listPtr = new double[MAX_SIZE];
            count = getListInput(&inputChoice, listPtr, MAX_SIZE, &inputFile);

            if (count <= 0)
            {
                // MEMORY MANAGEMENT: Free memory before continuing
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            g_isSorted = isSorted(listPtr, count);
            num1 = num2 = 0;

            result = calculate(&choice, &op, &num1, &num2, listPtr, count);
            sendOutput(&outputChoice, &num1, &op, &num2, &result, &outputFile);

            // MEMORY MANAGEMENT: Free memory after use
            delete[] listPtr;
            listPtr = nullptr;
        }
        // Handle sorting (bubble sort, selection sort)
        else if (choice == 10 || choice == 11)
        {
            selectIO(&inputChoice, &outputChoice);

            // DYNAMIC MEMORY: Allocate memory for list
            listPtr = new double[MAX_SIZE];
            count = getListInput(&inputChoice, listPtr, MAX_SIZE, &inputFile);

            if (count <= 0)
            {
                // MEMORY MANAGEMENT: Free memory before continuing
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            g_isSorted = isSorted(listPtr, count);

            // Show the unsorted list
            printList(listPtr, count, string("Original List"), &outputChoice, &outputFile, "Unsorted file name: ");

            if (choice == 10)
            {
                bubbleSort(listPtr, count);
                printList(listPtr, count, string("List (Bubble Sorted)"), &outputChoice, &outputFile, "Sorted file name: ");
            }
            else
            {
                selectionSort(listPtr, count);
                printList(listPtr, count, string("List (Selection Sorted)"), &outputChoice, &outputFile, "Sorted file name: ");
            }

            // MEMORY MANAGEMENT: Free memory after use
            delete[] listPtr;
            listPtr = nullptr;
        }
        // Handle searching (linear search, binary search)
        else if (choice == 12 || choice == 13)
        {
            selectIO(&inputChoice, &outputChoice);

            // DYNAMIC MEMORY: Allocate memory for list
            listPtr = new double[MAX_SIZE];
            count = getListInput(&inputChoice, listPtr, MAX_SIZE, &inputFile);

            if (count <= 0)
            {
                // MEMORY MANAGEMENT: Free memory before continuing
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            g_isSorted = isSorted(listPtr, count);

            double targetValue = 0;
            int index = -1;

            cout << "Enter the value to search for: ";
            if (!(cin >> targetValue))
            {
                cerr << errorMessage << " (invalid target)" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                // MEMORY MANAGEMENT: Free memory before continuing
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            // Perform linear search
            if (choice == 12)
            {
                index = linearSearch(listPtr, count, targetValue);
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
                        selectionSort(listPtr, count);
                        cout << "List has been sorted for binary search." << endl;
                    }
                    else
                    {
                        cout << "Binary search cancelled - list must be sorted to perform binary search." << endl;
                        // MEMORY MANAGEMENT: Free memory before continuing
                        delete[] listPtr;
                        listPtr = nullptr;
                        continue;
                    }
                }
                index = binarySearch(listPtr, count, targetValue);
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

            if (*(&outputChoice) == 'c')
            {
                cout << "\n--- Result ---" << endl;
                cout << resultLine << endl;
            }
            else if (*(&outputChoice) == 'f')
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

            // MEMORY MANAGEMENT: Free memory after use
            delete[] listPtr;
            listPtr = nullptr;
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
// REFACTORED: Uses pointers instead of references
double calculate(int* choicePtr, char* opPtr, double* n1Ptr, double* n2Ptr, double* listPtr, int size)
{
    double r = 0;
    // POINTER USAGE: Dereference pointers to access values
    switch (*choicePtr)
    {
    case 1:
        *opPtr = '+';
        r = add(*n1Ptr, *n2Ptr);
        break;
    case 2:
        *opPtr = '-';
        r = subtract(*n1Ptr, *n2Ptr);
        break;
    case 3:
        *opPtr = '*';
        r = multiply(*n1Ptr, *n2Ptr);
        break;
    case 4:
        *opPtr = '/';
        r = divide(*n1Ptr, *n2Ptr);
        break;
    case 5:
        *opPtr = 'S';
        r = sum(listPtr, size);
        break;
    case 6:
        *opPtr = 'A';
        r = mean(listPtr, size);
        break;
    case 7:
        *opPtr = 'D';
        r = median(listPtr, size);
        break;
    case 8:
        *opPtr = 'm';
        r = min(listPtr, size);
        break;
    case 9:
        *opPtr = 'X';
        r = max(listPtr, size);
        break;

    default:
        cout << "\n *** Invalid choice. ***" << endl;
    }

    return r;
}

// Output the result to console or file
// REFACTORED: Uses pointers instead of references
int sendOutput(char* outputChoicePtr, double* n1Ptr, char* opPtr, double* n2Ptr, double* resultPtr, ofstream* f)
{
    string outLine;
    // POINTER USAGE: Dereference pointers to access values
    if (*opPtr == '+' || *opPtr == '-' || *opPtr == '*' || *opPtr == '/')
    {
        outLine = to_string(*n1Ptr) + " " + *opPtr + " " + to_string(*n2Ptr) + " = " + to_string(*resultPtr);
    }
    else
    {
        string label;
        switch (*opPtr)
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
        outLine = label + " = " + to_string(*resultPtr);
    }

    if (*outputChoicePtr == 'c')
    {
        cout << outLine << endl;
    }
    else if (*outputChoicePtr == 'f')
    {
        string filename;
        cout << outputFileNamePrompt;
        cin >> filename;
        f->open(filename);
        if (!(*f))
        {
            cerr << errorMessage << endl;
            return -1;
        }
        *f << outLine << endl;
        f->close();
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
// POINTER ARITHMETIC: Uses pointer arithmetic instead of array indexing
double sum(const double* list, int size)
{
    double s = 0.0;
    // POINTER ARITHMETIC: Create a copy of the pointer for safe iteration
    const double* p = list;
    for (int i = 0; i < size; ++i)
    {
        // POINTER ARITHMETIC: Dereference and increment pointer
        s += *p;
        p++;
    }
    return s;
}

// Calculate the average of numbers in a list
double mean(const double* list, int size)
{
    if (size <= 0)
        return 0.0;
    return sum(list, size) / static_cast<double>(size);
}

// Find the middle value in a list
double median(double* list, int size)
{
    if (size <= 0)
        return 0.0;
    // DYNAMIC MEMORY: Create temporary copy for sorting (median needs non-destructive sort)
    double* tmp = new double[size];
    for (int i = 0; i < size; ++i)
    {
        tmp[i] = list[i];
    }
    sort(tmp, tmp + size);
    double result;
    if (size % 2 == 1)
    {
        result = tmp[size / 2];
    }
    else
    {
        double a = tmp[size / 2 - 1];
        double b = tmp[size / 2];
        result = (a + b) / 2.0;
    }
    // MEMORY MANAGEMENT: Free temporary array
    delete[] tmp;
    return result;
}

// Find the smallest number in a list
// POINTER ARITHMETIC: Uses pointer arithmetic instead of array indexing
double min(const double* list, int size)
{
    if (size <= 0)
        return 0.0;
    // POINTER ARITHMETIC: Initialize with first element
    double m = *list;
    const double* p = list + 1; // Start from second element
    for (int i = 1; i < size; ++i)
    {
        // POINTER ARITHMETIC: Dereference pointer to compare
        if (*p < m)
            m = *p;
        p++; // Move to next element
    }
    return m;
}

// Find the largest number in a list
double max(const double* list, int size)
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
// REFACTORED: Uses pointers instead of references
void displayMenu(int* choicePtr)
{
    while (true)
    {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;
        // POINTER USAGE: Dereference pointer for input
        if (!(cin >> *choicePtr))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 14." << endl;
            continue;
        }
        if (*choicePtr >= 1 && *choicePtr <= 14)
            break;
        cout << "Choice out of range. Try again." << endl;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Read a list from keyboard or file
// REFACTORED: Uses pointers and dynamic arrays instead of vector
int getListInput(char* inputChoicePtr, double* list, int maxSize, ifstream* f)
{
    int count = 0;
    // POINTER USAGE: Dereference pointer to access value
    if (*inputChoicePtr == 'k')
    {
        cout << "Enter numbers one at a time. Enter -1 to finish (sentinel)." << endl;
        double v = 0;
        while (count < maxSize)
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
            list[count] = v;
            count++;
        }
        if (count == 0)
        {
            cerr << "No numbers entered." << endl;
            return -1;
        }
    }
    else if (*inputChoicePtr == 'f')
    {
        string filename;
        while (true)
        {
            cout << inputFileNamePrompt;
            cin >> filename;
            f->clear();
            f->open(filename);
            if (!f->is_open())
            {
                cerr << errorMessage << " (could not open file)" << endl;
            }
            else
                break;
        }
        // Read numbers from file (supports plain numbers or "index: value" format)
        string line;
        while (getline(*f, line) && count < maxSize)
        {
            if (line.empty())
                continue;
            size_t colonPos = line.find(':');
            string toParse = (colonPos != string::npos) ? line.substr(colonPos + 1) : line;

            istringstream ss(toParse);
            double value;
            while (ss >> value && count < maxSize)
            {
                list[count] = value;
                count++;
            }
        }
        if (count == 0)
        {
            cerr << "No numbers found in file." << endl;
            f->close();
            return -1;
        }
        f->close();
    }
    else
    {
        cerr << errorMessage << endl;
        return -1;
    }
    return count;
}

// Choose input (k/f) and output (c/f)
// REFACTORED: Uses pointers instead of references
void selectIO(char* inputChoicePtr, char* outputChoicePtr)
{
    // POINTER USAGE: Dereference pointers to access and modify values
    while (*inputChoicePtr != 'k' && *inputChoicePtr != 'f')
    {
        cout << inputMethods;
        cin >> *inputChoicePtr;
        *inputChoicePtr = tolower(*inputChoicePtr);
    }
    while (*outputChoicePtr != 'c' && *outputChoicePtr != 'f')
    {
        cout << outputMethods;
        cin >> *outputChoicePtr;
        *outputChoicePtr = tolower(*outputChoicePtr);
    }
}

// Read two numbers from keyboard or file
// REFACTORED: Uses pointers instead of references
int getInput(char* inputChoicePtr, double* n1Ptr, double* n2Ptr, ifstream* f)
{
    // POINTER USAGE: Dereference pointers to access values
    if (*inputChoicePtr == 'k')
    {
        cout << firstNumberPrompt;
        if (!(cin >> *n1Ptr)) return -1;
        cout << secondNumberPrompt;
        if (!(cin >> *n2Ptr)) return -1;
    }
    else if (*inputChoicePtr == 'f')
    {
        string filename;
        while (true)
        {
            cout << inputFileNamePrompt;
            cin >> filename;
            f->clear();
            f->open(filename);
            if (!f->is_open())
            {
                cerr << errorMessage << endl;
            }
            else
            {
                break;
            }
        }
        if (!(*f >> *n1Ptr >> *n2Ptr))
        {
            cerr << errorMessage << " (failed to read two numbers)" << endl;
            f->close();
            return -1;
        }
        f->close();
    }
    else
    {
        cerr << errorMessage << endl;
        return -1;
    }
    return 0;
}

// Print list to console or file
// REFACTORED: Uses pointers instead of vector and references
void printList(const double* list, int size, const string& title, char* outputChoicePtr, ofstream* f, const string& filePrompt)
{
    // POINTER USAGE: Dereference pointer to access value
    if (*outputChoicePtr == 'c')
    {
        cout << "\n--- " << title << " ---" << endl;
        for (int i = 0; i < size; ++i)
        {
            cout << i << ": " << list[i] << endl;
        }
    }
    else if (*outputChoicePtr == 'f')
    {
        string filename;
        cout << filePrompt;
        cin >> filename;
        f->open(filename);
        if (!(*f))
        {
            cerr << errorMessage << endl;
            return;
        }
        for (int i = 0; i < size; ++i)
        {
            *f << i << ": " << list[i] << endl;
        }
        f->close();
    }
}

// Bubble sort (ascending)
// REFACTORED: Uses pointers instead of vector
void bubbleSort(double* arr, int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = 0; j < size - i - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
    g_isSorted = true;
}

// Selection sort (ascending)
// REFACTORED: Uses pointers instead of vector
void selectionSort(double* arr, int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        int minIdx = i;
        for (int j = i + 1; j < size; ++j)
            if (arr[j] < arr[minIdx])
                minIdx = j;
        if (minIdx != i)
            swap(arr[i], arr[minIdx]);
    }
    g_isSorted = true;
}

// Linear search - returns index or -1
// REFACTORED: Uses pointers instead of vector
int linearSearch(const double* arr, int size, double value)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] == value)
            return i;
    return -1;
}

// Binary search assumes arr is sorted ascending - returns index or -1
// REFACTORED: Uses pointers instead of vector
int binarySearch(const double* arr, int size, double value)
{
    int left = 0;
    int right = size - 1;
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

// Check whether an array is sorted in ascending order
// REFACTORED: Uses pointers instead of vector
bool isSorted(const double* arr, int size)
{
    for (int i = 1; i < size; ++i)
    {
        if (arr[i - 1] > arr[i])
            return false;
    }
    return true;
}
