#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstring>

using namespace std;

enum MenuChoice
{
    ADD = 1,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    SUM,
    MEAN,
    MEDIAN,
    MIN,
    MAX,
    BUBBLE_SORT,
    SELECTION_SORT,
    LINEAR_SEARCH,
    BINARY_SEARCH,
    C_STRING_COMPARE,
    STRING_COMPARE,
    SAVE_LIST,
    LOAD_LIST,
    VIEW_RECORD,
    EXIT
};

struct DataItem
{
    int id;
    double value;
};

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
                           "--- String Operations ---\n"
                           "14. C-String Comparison\n"
                           "15. std::string Comparison\n"
                           "--- File Operations ---\n"
                           "16. Save List to Binary File\n"
                           "17. Load List from Binary File & Run Stats\n"
                           "18. View Single Record from Binary File\n"
                           "--- System ---\n"
                           "19. Exit\n";
const char menuTitle[] = "Calculator Menu";
const char menuPrompt[] = "Please input an option from the above menu (1-19)";
const char repeatPrompt[] = "Would you like to perform another calculation? (Y/N): ";
const char inputMethods[] = "Select input method - keyboard (k) or file (f): ";
const char outputMethods[] = "Select output method - console (c) or file (f): ";
const char idPrompt[] = "Enter ID (or -1 to finish): ";
const char valuePrompt[] = "Enter Value: ";
const char invalidIdError[] = " (invalid ID)";
const char firstNumberPrompt[] = "Enter first number: ";
const char secondNumberPrompt[] = "Enter second number: ";
const char inputFileNamePrompt[] = "Enter input file name: ";
const char outputFileNamePrompt[] = "Enter output file name: ";
const char errorMessage[] = "Error, please try again.";
const char exitMessage[] = "Thank you for using the Simple Calculator!";
const char invalidChoiceMessage[] = "\n *** Invalid choice. ***";
const char searchPrompt[] = "Enter the value to search for: ";
const char invalidTargetError[] = " (invalid target)";
const char sortPromptMessage[] = "List is not sorted. Binary search requires a sorted list. Sort now? (y/n): ";
const char sortedMessage[] = "List has been sorted for binary search.";
const char binarySearchCancelledMessage[] = "Binary search cancelled - list must be sorted to perform binary search.";
const char valueFoundPrefix[] = "Value ";
const char valueFoundSuffix[] = " was FOUND at index ";
const char valueNotFoundSuffix[] = " was NOT FOUND.";
const char resultHeader[] = "\n--- Result ---";
const char openOutputFileError[] = " (could not open output file)";
const char originalListTitle[] = "Original List";
const char bubbleSortedListTitle[] = "List (Bubble Sorted)";
const char selectionSortedListTitle[] = "List (Selection Sorted)";
const char unsortedFilePrompt[] = "Unsorted file name: ";
const char sortedFilePrompt[] = "Sorted file name: ";
const char cStringComparisonTitle[] = "\n--- C-String Comparison ---";
const char stringComparisonTitle[] = "\n--- std::string Comparison ---";
const char enterFirstStringPrompt[] = "Enter first string: ";
const char enterSecondStringPrompt[] = "Enter second string: ";
const char comparisonResultHeader[] = "\nComparison Result:";
const char string1Label[] = "String 1: \"";
const char string2Label[] = "String 2: \"";
const char lessThanMessage[] = "String 1 is lexicographically LESS THAN String 2";
const char greaterThanMessage[] = "String 1 is lexicographically GREATER THAN String 2";
const char equalToMessage[] = "String 1 is lexicographically EQUAL TO String 2";
const char divideByZeroError[] = "cannot divide by zero";
const char invalidInputMessage[] = "Invalid input. Please enter a number between 1 and 19.";
const char choiceOutOfRangeMessage[] = "Choice out of range. Try again.";
const char dataFileName[] = "dataset.bin";
const char saveListTitle[] = "\n--- Save List to Binary File ---";
const char loadListTitle[] = "\n--- Load List from Binary File & Run Stats ---";
const char viewRecordTitle[] = "\n--- View Single Record ---";
const char recordIndexPrompt[] = "Enter record index (0-based): ";
const char noDataFileError[] = "No data file found. Please save a list first.";
const char invalidRecordError[] = " (invalid record index)";
const char recordNotFoundError[] = "Record not found. Index may be out of bounds.";
const char listSavedMessage[] = "List successfully saved to binary file.";
const char recordLabel[] = "Record ";
const char statsHeader[] = "\n--- Statistics for Loaded Data ---";
const char enterNumbersPrompt[] = "Enter numbers one at a time. Enter -1 to finish (sentinel).";
const char numberPrompt[] = "Number: ";
const char invalidNumberError[] = " (invalid number)";
const char convertNumberError[] = " (could not convert to number)";
const char noNumbersEnteredError[] = "No numbers entered.";
const char openFileError[] = " (could not open file)";
const char noNumbersInFileError[] = "No numbers found in file.";
const char sumLabel[] = "Sum";
const char meanLabel[] = "Mean";
const char medianLabel[] = "Median";
const char minLabel[] = "Min";
const char maxLabel[] = "Max";
const char resultLabel[] = "Result";
const char readTwoNumbersError[] = " (failed to read two numbers)";
const int MAX_SIZE = 100;

// Menu and input/output function prototypes
void displayMenu(MenuChoice* choicePtr);
void selectIO(char* inputChoicePtr, char* outputChoicePtr);
int getInput(char* inputChoicePtr, double* n1Ptr, double* n2Ptr, ifstream* f);
int getListInput(char* inputChoicePtr, DataItem* list, int maxSize, ifstream* f);
double calculate(MenuChoice* choicePtr, char* opPtr, double* n1Ptr, double* n2Ptr, DataItem* listPtr, int size);
int sendOutput(char* outputChoicePtr, double* n1Ptr, char* opPtr, double* n2Ptr, double* resultPtr, ofstream* f);
bool isNumeric(string s);

// Sorting and searching function prototypes
bool g_isSorted = false;
void printList(const DataItem* list, int size, const string& title, char* outputChoicePtr, ofstream* f, const string& filePrompt = string(outputFileNamePrompt));
void bubbleSort(DataItem* arr, int size);
void selectionSort(DataItem* arr, int size);
int linearSearch(const DataItem* arr, int size, double value);
int binarySearch(const DataItem* arr, int size, double value);
bool isSorted(const DataItem* arr, int size);

// Math function prototypes
double add(double, double);
double subtract(double, double);
double multiply(double, double);
double divide(double, double);
double sum(const DataItem* list, int size);
double mean(const DataItem* list, int size);
double median(DataItem* list, int size);
double min(const DataItem* list, int size);
double max(const DataItem* list, int size);

int main()
{
    MenuChoice choice = ADD;
    double num1 = 0, num2 = 0, result = 0;
    char inputChoice = '\0', outputChoice = '\0', op = '\0', repeat = 'y';
    DataItem* listPtr = nullptr;
    int count = 0;
    ifstream inputFile;
    ofstream outputFile;

    do
    {
        displayMenu(&choice);

        if (choice == EXIT)
        {
            cout << endl
                 << exitMessage << endl;
            break;
        }

        // Basic math operations
        if (choice >= ADD && choice <= DIVIDE)
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
        else if (choice >= SUM && choice <= MAX)
        {
            selectIO(&inputChoice, &outputChoice);

            // Create array for list of DataItem structs
            listPtr = new DataItem[MAX_SIZE];
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
        else if (choice == BUBBLE_SORT || choice == SELECTION_SORT)
        {
            selectIO(&inputChoice, &outputChoice);

            // Create array for list of DataItem structs
            listPtr = new DataItem[MAX_SIZE];
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
            printList(listPtr, count, string(originalListTitle), &outputChoice, &outputFile, unsortedFilePrompt);

            if (choice == BUBBLE_SORT)
            {
                bubbleSort(listPtr, count);
                printList(listPtr, count, string(bubbleSortedListTitle), &outputChoice, &outputFile, sortedFilePrompt);
            }
            else
            {
                selectionSort(listPtr, count);
                printList(listPtr, count, string(selectionSortedListTitle), &outputChoice, &outputFile, sortedFilePrompt);
            }

            // Clean up memory
            delete[] listPtr;
            listPtr = nullptr;
        }
        // Search operations
        else if (choice == LINEAR_SEARCH || choice == BINARY_SEARCH)
        {
            selectIO(&inputChoice, &outputChoice);

            // Create array for list of DataItem structs
            listPtr = new DataItem[MAX_SIZE];
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

            cout << searchPrompt;
            if (!(cin >> targetValue))
            {
                cerr << errorMessage << invalidTargetError << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                // Clean up memory
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            // Do linear search
            if (choice == LINEAR_SEARCH)
            {
                index = linearSearch(listPtr, count, targetValue);
            }
            // Do binary search (needs sorted list)
            else
            {
                if (!g_isSorted)
                {
                    char sortNow = 'n';
                    cout << sortPromptMessage;
                    cin >> sortNow;
                    sortNow = tolower(sortNow);
                    if (sortNow == 'y')
                    {
                        selectionSort(listPtr, count);
                        cout << sortedMessage << endl;
                    }
                    else
                    {
                        cout << binarySearchCancelledMessage << endl;
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
                resultLine = string(valueFoundPrefix) + to_string(targetValue) + string(valueFoundSuffix) + to_string(index) + string(".");
            }
            else
            {
                resultLine = string(valueFoundPrefix) + to_string(targetValue) + string(valueNotFoundSuffix);
            }

            if (*(&outputChoice) == 'c')
            {
                cout << resultHeader << endl;
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
                    cerr << errorMessage << openOutputFileError << endl;
                }
                else
                {
                    outputFile << resultHeader << endl;
                    outputFile << resultLine << endl;
                    outputFile.close();
                }
            }

            // Clean up memory
            delete[] listPtr;
            listPtr = nullptr;
        }
        // String comparison operations
        else if (choice == C_STRING_COMPARE || choice == STRING_COMPARE)
        {
            calculate(&choice, &op, &num1, &num2, nullptr, 0);
        }
        // File operations
        else if (choice == SAVE_LIST || choice == LOAD_LIST || choice == VIEW_RECORD)
        {
            calculate(&choice, &op, &num1, &num2, nullptr, 0);
        }
        else
        {
            cout << invalidChoiceMessage << endl;
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
double calculate(MenuChoice* choicePtr, char* opPtr, double* n1Ptr, double* n2Ptr, DataItem* listPtr, int size)
{
    double r = 0;
    switch (*choicePtr)
    {
    case ADD:
        *opPtr = '+';
        r = add(*n1Ptr, *n2Ptr);
        break;
    case SUBTRACT:
        *opPtr = '-';
        r = subtract(*n1Ptr, *n2Ptr);
        break;
    case MULTIPLY:
        *opPtr = '*';
        r = multiply(*n1Ptr, *n2Ptr);
        break;
    case DIVIDE:
        *opPtr = '/';
        r = divide(*n1Ptr, *n2Ptr);
        break;
    case SUM:
        *opPtr = 'S';
        r = sum(listPtr, size);
        break;
    case MEAN:
        *opPtr = 'A';
        r = mean(listPtr, size);
        break;
    case MEDIAN:
        *opPtr = 'D';
        r = median(listPtr, size);
        break;
    case MIN:
        *opPtr = 'm';
        r = min(listPtr, size);
        break;
    case MAX:
        *opPtr = 'X';
        r = max(listPtr, size);
        break;

    case C_STRING_COMPARE:
        {
            cout << cStringComparisonTitle << endl;
            char str1[100];
            char str2[100];

            cout << enterFirstStringPrompt;
            cin.getline(str1, 100);

            cout << enterSecondStringPrompt;
            cin.getline(str2, 100);

            int result = strcmp(str1, str2);

            cout << comparisonResultHeader << endl;
            cout << string1Label << str1 << "\"" << endl;
            cout << string2Label << str2 << "\"" << endl;

            if (result < 0)
            {
                cout << lessThanMessage << endl;
            }
            else if (result > 0)
            {
                cout << greaterThanMessage << endl;
            }
            else
            {
                cout << equalToMessage << endl;
            }
            *opPtr = 'C'; // C-string comparison marker
        }
        break;

    case STRING_COMPARE:
        {
            cout << stringComparisonTitle << endl;
            string str1, str2;

            cout << enterFirstStringPrompt;
            getline(cin, str1);

            cout << enterSecondStringPrompt;
            getline(cin, str2);

            cout << comparisonResultHeader << endl;
            cout << string1Label << str1 << "\"" << endl;
            cout << string2Label << str2 << "\"" << endl;

            if (str1 < str2)
            {
                cout << lessThanMessage << endl;
            }
            else if (str1 > str2)
            {
                cout << greaterThanMessage << endl;
            }
            else
            {
                cout << equalToMessage << endl;
            }
            *opPtr = 'S'; // std::string comparison marker
        }
        break;

    case SAVE_LIST:
        {
            cout << saveListTitle << endl;

            // Allocate array for list
            listPtr = new DataItem[MAX_SIZE];

            // Get input from keyboard
            cout << enterNumbersPrompt << endl;
            int id = 0;
            double v = 0;
            int count = 0;

            while (count < MAX_SIZE)
            {
                // Read ID
                cout << idPrompt;
                string input;
                getline(cin, input);

                // Validate input using isNumeric
                if (!isNumeric(input))
                {
                    cerr << errorMessage << invalidIdError << endl;
                    continue;
                }

                try
                {
                    id = stoi(input);
                }
                catch (...)
                {
                    cerr << errorMessage << invalidIdError << endl;
                    continue;
                }

                // Check for sentinel value
                if (id == -1)
                    break;

                // Read Value
                cout << valuePrompt;
                getline(cin, input);

                // Validate input using isNumeric
                if (!isNumeric(input))
                {
                    cerr << errorMessage << invalidNumberError << endl;
                    continue;
                }

                try
                {
                    v = stod(input);
                }
                catch (...)
                {
                    cerr << errorMessage << convertNumberError << endl;
                    continue;
                }

                // Store in DataItem struct
                listPtr[count].id = id;
                listPtr[count].value = v;
                count++;
            }

            if (count > 0)
            {
                // Open binary file for writing
                fstream dataFile;
                dataFile.open(dataFileName, ios::out | ios::binary);

                if (!dataFile)
                {
                    cerr << errorMessage << openFileError << endl;
                }
                else
                {
                    // Write entire array to file
                    dataFile.write(reinterpret_cast<char*>(listPtr), count * sizeof(DataItem));
                    dataFile.close();
                    cout << listSavedMessage << endl;
                }
            }
            else
            {
                cerr << noNumbersEnteredError << endl;
            }

            // Clean up memory
            delete[] listPtr;
            listPtr = nullptr;
            *opPtr = 'F'; // File operation marker
        }
        break;

    case LOAD_LIST:
        {
            cout << loadListTitle << endl;

            // Open binary file for reading
            fstream dataFile;
            dataFile.open(dataFileName, ios::in | ios::binary);

            if (!dataFile)
            {
                cerr << errorMessage << " " << noDataFileError << endl;
            }
            else
            {
                // Get file size
                dataFile.seekg(0, ios::end);
                long fileSize = dataFile.tellg();

                // Calculate number of records
                int numRecords = fileSize / sizeof(DataItem);

                if (numRecords > 0)
                {
                    // Allocate array
                    listPtr = new DataItem[numRecords];

                    // Seek to beginning
                    dataFile.seekg(0, ios::beg);

                    // Read entire file
                    dataFile.read(reinterpret_cast<char*>(listPtr), fileSize);

                    if (dataFile.fail())
                    {
                        cerr << errorMessage << openFileError << endl;
                    }
                    else
                    {
                        // Run all statistics
                        cout << statsHeader << endl;
                        cout << sumLabel << " = " << sum(listPtr, numRecords) << endl;
                        cout << meanLabel << " = " << mean(listPtr, numRecords) << endl;
                        cout << medianLabel << " = " << median(listPtr, numRecords) << endl;
                        cout << minLabel << " = " << min(listPtr, numRecords) << endl;
                        cout << maxLabel << " = " << max(listPtr, numRecords) << endl;
                    }

                    // Clean up memory
                    delete[] listPtr;
                    listPtr = nullptr;
                }
                else
                {
                    cerr << noNumbersInFileError << endl;
                }

                dataFile.close();
            }
            *opPtr = 'L'; // Load operation marker
        }
        break;

    case VIEW_RECORD:
        {
            cout << viewRecordTitle << endl;

            // Open binary file for reading
            fstream dataFile;
            dataFile.open(dataFileName, ios::in | ios::binary);

            if (!dataFile)
            {
                cerr << errorMessage << " " << noDataFileError << endl;
            }
            else
            {
                // Get record index from user
                cout << recordIndexPrompt;
                string input;
                getline(cin, input);

                if (!isNumeric(input))
                {
                    cerr << errorMessage << invalidRecordError << endl;
                }
                else
                {
                    try
                    {
                        int index = stoi(input);

                        // Seek to record position
                        dataFile.seekg(index * sizeof(DataItem), ios::beg);

                        // Read single record
                        DataItem temp;
                        dataFile.read(reinterpret_cast<char*>(&temp), sizeof(DataItem));

                        if (dataFile.fail())
                        {
                            cerr << errorMessage << " " << recordNotFoundError << endl;
                        }
                        else
                        {
                            cout << recordLabel << index << ": ID=" << temp.id << " Value=" << temp.value << endl;
                        }
                    }
                    catch (...)
                    {
                        cerr << errorMessage << invalidRecordError << endl;
                    }
                }

                dataFile.close();
            }
            *opPtr = 'V'; // View operation marker
        }
        break;

    default:
        cout << invalidChoiceMessage << endl;
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
            label = sumLabel;
            break;
        case 'A':
            label = meanLabel;
            break;
        case 'D':
            label = medianLabel;
            break;
        case 'm':
            label = minLabel;
            break;
        case 'X':
            label = maxLabel;
            break;
        default:
            label = resultLabel;
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
        cout << divideByZeroError << endl;
        return 0;
    }
    return num1 / num2;
}

// Add up all numbers in a list
double sum(const DataItem* list, int size)
{
    double s = 0.0;
    for (int i = 0; i < size; ++i)
    {
        s += list[i].value;
    }
    return s;
}

// Find the average of numbers in a list
double mean(const DataItem* list, int size)
{
    if (size <= 0)
        return 0.0;
    return sum(list, size) / static_cast<double>(size);
}

// Find the middle value in a list
double median(DataItem* list, int size)
{
    if (size <= 0)
        return 0.0;
    // Make a copy of the values to sort without changing the original
    double* tmp = new double[size];
    for (int i = 0; i < size; ++i)
    {
        tmp[i] = list[i].value;
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
double min(const DataItem* list, int size)
{
    if (size <= 0)
        return 0.0;
    double m = list[0].value;
    for (int i = 1; i < size; ++i)
    {
        if (list[i].value < m)
            m = list[i].value;
    }
    return m;
}

// Find the largest number in a list
double max(const DataItem* list, int size)
{
    if (size <= 0)
        return 0.0;
    double m = list[0].value;
    for (int i = 1; i < size; ++i)
        if (list[i].value > m)
            m = list[i].value;
    return m;
}

// Display menu and get user's choice
void displayMenu(MenuChoice* choicePtr)
{
    string input;
    int intChoice = 0;
    while (true)
    {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;

        getline(cin, input);

        // Validate the input using isNumeric
        if (!isNumeric(input))
        {
            cout << invalidInputMessage << endl;
            continue;
        }

        try
        {
            intChoice = stoi(input);
        }
        catch (...)
        {
            cout << invalidInputMessage << endl;
            continue;
        }

        // Check if choice is in valid range
        if (intChoice >= 1 && intChoice <= 19)
        {
            *choicePtr = static_cast<MenuChoice>(intChoice);
            break;
        }

        cout << choiceOutOfRangeMessage << endl;
    }
}

// Read a list from keyboard or file
int getListInput(char* inputChoicePtr, DataItem* list, int maxSize, ifstream* f)
{
    int count = 0;
    if (*inputChoicePtr == 'k')
    {
        cout << enterNumbersPrompt << endl;
        int id = 0;
        double v = 0;
        while (count < maxSize)
        {
            // Read ID
            cout << idPrompt;
            string input;
            getline(cin, input);

            // Validate input using isNumeric
            if (!isNumeric(input))
            {
                cerr << errorMessage << invalidIdError << endl;
                continue;
            }

            try
            {
                id = stoi(input);
            }
            catch (...)
            {
                cerr << errorMessage << invalidIdError << endl;
                continue;
            }

            // Check for sentinel value
            if (id == -1)
                break;

            // Read Value
            cout << valuePrompt;
            getline(cin, input);

            // Validate input using isNumeric
            if (!isNumeric(input))
            {
                cerr << errorMessage << invalidNumberError << endl;
                continue;
            }

            try
            {
                v = stod(input);
            }
            catch (...)
            {
                cerr << errorMessage << convertNumberError << endl;
                continue;
            }

            // Store in DataItem struct
            list[count].id = id;
            list[count].value = v;
            count++;
        }
        if (count == 0)
        {
            cerr << noNumbersEnteredError << endl;
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
                cerr << errorMessage << openFileError << endl;
            }
            else
                break;
        }
        // Read DataItem records from file (supports "id value" or "index: id value" format)
        string line;
        while (getline(*f, line) && count < maxSize)
        {
            if (line.empty())
                continue;

            // Remove "index: " prefix if present
            size_t colonPos = line.find(':');
            string toParse = (colonPos != string::npos) ? line.substr(colonPos + 1) : line;

            istringstream ss(toParse);
            int id;
            double value;

            // Read both ID and value
            if (ss >> id >> value)
            {
                list[count].id = id;
                list[count].value = value;
                count++;
            }
        }
        if (count == 0)
        {
            cerr << noNumbersInFileError << endl;
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
    string input;

    while (*inputChoicePtr != 'k' && *inputChoicePtr != 'f')
    {
        cout << inputMethods;
        getline(cin, input);

        if (input.empty())
            continue;

        *inputChoicePtr = tolower(input[0]);
    }

    while (*outputChoicePtr != 'c' && *outputChoicePtr != 'f')
    {
        cout << outputMethods;
        getline(cin, input);

        if (input.empty())
            continue;

        *outputChoicePtr = tolower(input[0]);
    }
}

// Read two numbers from keyboard or file
int getInput(char* inputChoicePtr, double* n1Ptr, double* n2Ptr, ifstream* f)
{
    if (*inputChoicePtr == 'k')
    {
        string input;

        // Get first number with robust validation
        while (true)
        {
            cout << firstNumberPrompt;
            getline(cin, input);

            if (!isNumeric(input))
            {
                cerr << errorMessage << invalidNumberError << endl;
                continue;
            }

            try
            {
                *n1Ptr = stod(input);
                break;
            }
            catch (...)
            {
                cerr << errorMessage << convertNumberError << endl;
            }
        }

        // Get second number with robust validation
        while (true)
        {
            cout << secondNumberPrompt;
            getline(cin, input);

            if (!isNumeric(input))
            {
                cerr << errorMessage << invalidNumberError << endl;
                continue;
            }

            try
            {
                *n2Ptr = stod(input);
                break;
            }
            catch (...)
            {
                cerr << errorMessage << convertNumberError << endl;
            }
        }
    }
    else if (*inputChoicePtr == 'f')
    {
        string filename;
        while (true)
        {
            cout << inputFileNamePrompt;
            getline(cin, filename);
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
            cerr << errorMessage << readTwoNumbersError << endl;
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
void printList(const DataItem* list, int size, const string& title, char* outputChoicePtr, ofstream* f, const string& filePrompt)
{
    if (*outputChoicePtr == 'c')
    {
        cout << "\n--- " << title << " ---" << endl;
        for (int i = 0; i < size; ++i)
        {
            cout << i << ": ID=" << list[i].id << " Value=" << list[i].value << endl;
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
            *f << i << ": ID=" << list[i].id << " Value=" << list[i].value << endl;
        }
        f->close();
    }
}

// Sort list from smallest to largest using bubble sort
void bubbleSort(DataItem* arr, int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = 0; j < size - i - 1; ++j)
        {
            if (arr[j].value > arr[j + 1].value)
                swap(arr[j], arr[j + 1]);
        }
    }
    g_isSorted = true;
}

// Sort list from smallest to largest using selection sort
void selectionSort(DataItem* arr, int size)
{
    for (int i = 0; i < size - 1; ++i)
    {
        int minIdx = i;
        for (int j = i + 1; j < size; ++j)
            if (arr[j].value < arr[minIdx].value)
                minIdx = j;
        if (minIdx != i)
            swap(arr[i], arr[minIdx]);
    }
    g_isSorted = true;
}

// Search through list one by one to find a value
int linearSearch(const DataItem* arr, int size, double value)
{
    for (int i = 0; i < size; ++i)
        if (arr[i].value == value)
            return i;
    return -1;
}

// Search through sorted list by dividing in half repeatedly
int binarySearch(const DataItem* arr, int size, double value)
{
    int left = 0;
    int right = size - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (arr[mid].value == value)
            return mid;
        else if (arr[mid].value < value)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// Check if list is sorted from smallest to largest
bool isSorted(const DataItem* arr, int size)
{
    for (int i = 1; i < size; ++i)
    {
        if (arr[i - 1].value > arr[i].value)
            return false;
    }
    return true;
}

// LAB 4: Robust input validation helper function
bool isNumeric(string s)
{
    // Return false if the string is empty
    if (s.empty())
        return false;

    bool hasDecimal = false;

    // Loop through each character in the string
    for (size_t i = 0; i < s.length(); ++i)
    {
        char c = s[i];

        // Allow leading negative sign
        if (i == 0 && c == '-')
        {
            // If the string is only "-", it's not valid
            if (s.length() == 1)
                return false;
            continue;
        }

        // Allow one decimal point
        if (c == '.')
        {
            // Only allow one decimal point
            if (hasDecimal)
                return false;
            hasDecimal = true;
            continue;
        }

        // Check if character is a digit
        if (!isdigit(c))
            return false;
    }

    return true;
}