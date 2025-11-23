#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

// Text shown to the user
const char menuOptions[] = "1. Add\n"
                           "2. Subtract\n"
                           "3. Multiply\n"
                           "4. Divide\n"
                           "5. Sum\n"
                           "6. Mean\n"
                           "7. Median\n"
                           "8. Min\n"
                           "9. Max\n"
                           "--- Algorithms ---\n"
                           "10. Bubble Sort\n"
                           "11. Selection Sort\n"
                           "12. Linear Search\n"
                           "13. Binary Search\n"
                           "--- System ---\n"
                           "14. Exit\n";
const char menuTitle[] = "Calculator Menu";
const char menuPrompt[] = "Please input an option from the above menu (1-14)";
const char repeatPrompt[] = "Would you like to perform another calculation? (Y/N): ";
const char inputMethods[] = "Select input method - keyboard (k) or file (f): ";
const char outputMethods[] = "Select output method - console (c) or file (f): ";
const char firstNumberPrompt[] = "Enter first number: ";
const char secondNumberPrompt[] = "Enter second number: ";
const char inputFileNamePrompt[] = "Enter input file name: ";
const char outputFileNamePrompt[] = "Enter output file name: ";
const char errorMessage[] = "Error, please try again.";
const int MAX_SIZE = 100;

// Menu and input/output function prototypes
void displayMenu(int* choicePtr);
void selectIO(char* inputChoicePtr, char* outputChoicePtr);
int getInput(char* inputChoicePtr, double* n1Ptr, double* n2Ptr, ifstream* f);
int getListInput(char* inputChoicePtr, double* list, int maxSize, ifstream* f);
double calculate(int* choicePtr, char* opPtr, double* n1Ptr, double* n2Ptr, double* listPtr, int size);
int sendOutput(char* outputChoicePtr, double* n1Ptr, char* opPtr, double* n2Ptr, double* resultPtr, ofstream* f);

// Sorting and searching function prototypes
bool g_isSorted = false;
void printList(const double* list, int size, const string& title, char* outputChoicePtr, ofstream* f, const string& filePrompt = string(outputFileNamePrompt));
void bubbleSort(double* arr, int size);
void selectionSort(double* arr, int size);
int linearSearch(const double* arr, int size, double value);
int binarySearch(const double* arr, int size, double value);
bool isSorted(const double* arr, int size);

// Math function prototypes
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
    double* listPtr = nullptr;
    int count = 0;
    ifstream inputFile;
    ofstream outputFile;

    do
    {
        displayMenu(&choice);

        if (choice == 14)
        {
            cout << endl
                 << "Thank you for using the Simple Calculator!" << endl;
            break;
        }

        // Basic math operations
        if (choice >= 1 && choice <= 4)
        {
            selectIO(&inputChoice, &outputChoice);
            if (getInput(&inputChoice, &num1, &num2, &inputFile) != 0)
            {
                continue;
            }

            result = calculate(&choice, &op, &num1, &num2, nullptr, 0);
            sendOutput(&outputChoice, &num1, &op, &num2, &result, &outputFile);
        }
        // Statistics operations
        else if (choice >= 5 && choice <= 9)
        {
            selectIO(&inputChoice, &outputChoice);

            // Create array for list of numbers
            listPtr = new double[MAX_SIZE];
            count = getListInput(&inputChoice, listPtr, MAX_SIZE, &inputFile);

            if (count <= 0)
            {
                // Clean up memory
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            g_isSorted = isSorted(listPtr, count);
            num1 = num2 = 0;

            result = calculate(&choice, &op, &num1, &num2, listPtr, count);
            sendOutput(&outputChoice, &num1, &op, &num2, &result, &outputFile);

            // Clean up memory
            delete[] listPtr;
            listPtr = nullptr;
        }
        // Sorting operations
        else if (choice == 10 || choice == 11)
        {
            selectIO(&inputChoice, &outputChoice);

            // Create array for list of numbers
            listPtr = new double[MAX_SIZE];
            count = getListInput(&inputChoice, listPtr, MAX_SIZE, &inputFile);

            if (count <= 0)
            {
                // Clean up memory
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            g_isSorted = isSorted(listPtr, count);

            // Show original list before sorting
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

            // Clean up memory
            delete[] listPtr;
            listPtr = nullptr;
        }
        // Search operations
        else if (choice == 12 || choice == 13)
        {
            selectIO(&inputChoice, &outputChoice);

            // Create array for list of numbers
            listPtr = new double[MAX_SIZE];
            count = getListInput(&inputChoice, listPtr, MAX_SIZE, &inputFile);

            if (count <= 0)
            {
                // Clean up memory
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
                // Clean up memory
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            // Do linear search
            if (choice == 12)
            {
                index = linearSearch(listPtr, count, targetValue);
            }
            // Do binary search (needs sorted list)
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
                        // Clean up memory
                        delete[] listPtr;
                        listPtr = nullptr;
                        continue;
                    }
                }
                index = binarySearch(listPtr, count, targetValue);
            }

            // Show search results
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

            // Clean up memory
            delete[] listPtr;
            listPtr = nullptr;
        }
        else
        {
            cout << "\n *** Invalid choice. ***" << endl;
            continue;
        }

        // Ask if user wants to do another calculation
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

// Do the calculation based on user's choice
double calculate(int* choicePtr, char* opPtr, double* n1Ptr, double* n2Ptr, double* listPtr, int size)
{
    double r = 0;
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

// Show the result on screen or save to file
int sendOutput(char* outputChoicePtr, double* n1Ptr, char* opPtr, double* n2Ptr, double* resultPtr, ofstream* f)
{
    string outLine;
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

// Add up all numbers in a list
double sum(const double* list, int size)
{
    double s = 0.0;
    const double* p = list;
    for (int i = 0; i < size; ++i)
    {
        s += *p;
        p++;
    }
    return s;
}

// Find the average of numbers in a list
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
    // Make a copy of the list to sort without changing the original
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
    // Free the temporary array
    delete[] tmp;
    return result;
}

// Find the smallest number in a list
double min(const double* list, int size)
{
    if (size <= 0)
        return 0.0;
    double m = *list;
    const double* p = list + 1;
    for (int i = 1; i < size; ++i)
    {
        if (*p < m)
            m = *p;
        p++;
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
void displayMenu(int* choicePtr)
{
    while (true)
    {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;
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
int getListInput(char* inputChoicePtr, double* list, int maxSize, ifstream* f)
{
    int count = 0;
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
void selectIO(char* inputChoicePtr, char* outputChoicePtr)
{
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
int getInput(char* inputChoicePtr, double* n1Ptr, double* n2Ptr, ifstream* f)
{
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
void printList(const double* list, int size, const string& title, char* outputChoicePtr, ofstream* f, const string& filePrompt)
{
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

// Sort list from smallest to largest using bubble sort
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

// Sort list from smallest to largest using selection sort
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

// Search through list one by one to find a value
int linearSearch(const double* arr, int size, double value)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] == value)
            return i;
    return -1;
}

// Search through sorted list by dividing in half repeatedly
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

// Check if list is sorted from smallest to largest
bool isSorted(const double* arr, int size)
{
    for (int i = 1; i < size; ++i)
    {
        if (arr[i - 1] > arr[i])
            return false;
    }
    return true;
}
