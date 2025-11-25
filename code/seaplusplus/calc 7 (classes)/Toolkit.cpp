#include "Toolkit.h"

// String constants
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

// Constructor - Initialize all private data members
Toolkit::Toolkit() {
    choice = ADD;
    num1 = 0.0;
    num2 = 0.0;
    result = 0.0;
    op = ' ';
    repeat = 'y';
    inputChoice = '\0';
    outputChoice = '\0';
    listPtr = nullptr;
    count = 0;
    isSortedFlag = false;
}

// Main program loop (from old main function)
void Toolkit::run() {
    do {
        displayMenu();

        if (choice == EXIT) {
            cout << endl << exitMessage << endl;
            break;
        }

        // Basic math operations
        if (choice >= ADD && choice <= DIVIDE) {
            selectIO();
            if (getInput() != 0) {
                continue;
            }
            result = calculate();
            sendOutput();
        }
        // Statistics operations
        else if (choice >= SUM && choice <= MAX) {
            selectIO();
            listPtr = new DataItem[MAX_SIZE];
            count = getListInput();

            if (count <= 0) {
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            isSortedFlag = isSorted(listPtr, count);
            num1 = num2 = 0;

            result = calculate();
            sendOutput();

            delete[] listPtr;
            listPtr = nullptr;
        }
        // Sorting operations
        else if (choice == BUBBLE_SORT || choice == SELECTION_SORT) {
            selectIO();
            listPtr = new DataItem[MAX_SIZE];
            count = getListInput();

            if (count <= 0) {
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            isSortedFlag = isSorted(listPtr, count);

            // Show original list before sorting
            printList(listPtr, count, string(originalListTitle), unsortedFilePrompt);

            if (choice == BUBBLE_SORT) {
                bubbleSort(listPtr, count);
                printList(listPtr, count, string(bubbleSortedListTitle), sortedFilePrompt);
            } else {
                selectionSort(listPtr, count);
                printList(listPtr, count, string(selectionSortedListTitle), sortedFilePrompt);
            }

            delete[] listPtr;
            listPtr = nullptr;
        }
        // Search operations
        else if (choice == LINEAR_SEARCH || choice == BINARY_SEARCH) {
            selectIO();
            listPtr = new DataItem[MAX_SIZE];
            count = getListInput();

            if (count <= 0) {
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            isSortedFlag = isSorted(listPtr, count);

            double targetValue = 0;
            int index = -1;

            cout << searchPrompt;
            if (!(cin >> targetValue)) {
                cerr << errorMessage << invalidTargetError << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                delete[] listPtr;
                listPtr = nullptr;
                continue;
            }

            if (choice == LINEAR_SEARCH) {
                index = linearSearch(listPtr, count, targetValue);
            } else {
                if (!isSortedFlag) {
                    char sortNow = 'n';
                    cout << sortPromptMessage;
                    cin >> sortNow;
                    sortNow = tolower(sortNow);
                    if (sortNow == 'y') {
                        selectionSort(listPtr, count);
                        cout << sortedMessage << endl;
                    } else {
                        cout << binarySearchCancelledMessage << endl;
                        delete[] listPtr;
                        listPtr = nullptr;
                        continue;
                    }
                }
                index = binarySearch(listPtr, count, targetValue);
            }

            // Show search results
            string resultLine;
            if (index != -1) {
                resultLine = string(valueFoundPrefix) + to_string(targetValue) +
                           string(valueFoundSuffix) + to_string(index) + string(".");
            } else {
                resultLine = string(valueFoundPrefix) + to_string(targetValue) +
                           string(valueNotFoundSuffix);
            }

            if (outputChoice == 'c') {
                cout << resultHeader << endl;
                cout << resultLine << endl;
            } else if (outputChoice == 'f') {
                string filename;
                cout << outputFileNamePrompt;
                cin >> filename;
                outputFile.open(filename);
                if (!outputFile) {
                    cerr << errorMessage << openOutputFileError << endl;
                } else {
                    outputFile << resultHeader << endl;
                    outputFile << resultLine << endl;
                    outputFile.close();
                }
            }

            delete[] listPtr;
            listPtr = nullptr;
        }
        // String comparison operations
        else if (choice == C_STRING_COMPARE || choice == STRING_COMPARE) {
            calculate();
        }
        // File operations
        else if (choice == SAVE_LIST || choice == LOAD_LIST || choice == VIEW_RECORD) {
            calculate();
        }
        else {
            cout << invalidChoiceMessage << endl;
            continue;
        }

        // Ask if user wants to do another calculation
        do {
            cout << endl << repeatPrompt;
            cin >> repeat;
            repeat = tolower(repeat);
        } while (repeat != 'y' && repeat != 'n');

    } while (repeat == 'y');
}

// Display menu and get user's choice
void Toolkit::displayMenu() {
    string input;
    int intChoice = 0;
    while (true) {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;

        getline(cin, input);

        if (!isNumeric(input)) {
            cout << invalidInputMessage << endl;
            continue;
        }

        try {
            intChoice = stoi(input);
        } catch (...) {
            cout << invalidInputMessage << endl;
            continue;
        }

        if (intChoice >= 1 && intChoice <= 19) {
            choice = static_cast<MenuChoice>(intChoice);
            break;
        }

        cout << choiceOutOfRangeMessage << endl;
    }
}

// Choose input (k/f) and output (c/f)
void Toolkit::selectIO() {
    string input;

    // Reset choices
    inputChoice = '\0';
    outputChoice = '\0';

    while (inputChoice != 'k' && inputChoice != 'f') {
        cout << inputMethods;
        getline(cin, input);

        if (input.empty())
            continue;

        inputChoice = tolower(input[0]);
    }

    while (outputChoice != 'c' && outputChoice != 'f') {
        cout << outputMethods;
        getline(cin, input);

        if (input.empty())
            continue;

        outputChoice = tolower(input[0]);
    }
}

// Read two numbers from keyboard or file
int Toolkit::getInput() {
    if (inputChoice == 'k') {
        string input;

        // Get first number
        while (true) {
            cout << firstNumberPrompt;
            getline(cin, input);

            if (!isNumeric(input)) {
                cerr << errorMessage << invalidNumberError << endl;
                continue;
            }

            try {
                num1 = stod(input);
                break;
            } catch (...) {
                cerr << errorMessage << convertNumberError << endl;
            }
        }

        // Get second number
        while (true) {
            cout << secondNumberPrompt;
            getline(cin, input);

            if (!isNumeric(input)) {
                cerr << errorMessage << invalidNumberError << endl;
                continue;
            }

            try {
                num2 = stod(input);
                break;
            } catch (...) {
                cerr << errorMessage << convertNumberError << endl;
            }
        }
    } else if (inputChoice == 'f') {
        string filename;
        while (true) {
            cout << inputFileNamePrompt;
            getline(cin, filename);
            inputFile.clear();
            inputFile.open(filename);
            if (!inputFile.is_open()) {
                cerr << errorMessage << endl;
            } else {
                break;
            }
        }
        if (!(inputFile >> num1 >> num2)) {
            cerr << errorMessage << readTwoNumbersError << endl;
            inputFile.close();
            return -1;
        }
        inputFile.close();
    } else {
        cerr << errorMessage << endl;
        return -1;
    }
    return 0;
}

// Read a list from keyboard or file
int Toolkit::getListInput() {
    int c = 0;
    if (inputChoice == 'k') {
        cout << enterNumbersPrompt << endl;
        int id = 0;
        double v = 0;
        while (c < MAX_SIZE) {
            cout << idPrompt;
            string input;
            getline(cin, input);

            if (!isNumeric(input)) {
                cerr << errorMessage << invalidIdError << endl;
                continue;
            }

            try {
                id = stoi(input);
            } catch (...) {
                cerr << errorMessage << invalidIdError << endl;
                continue;
            }

            if (id == -1)
                break;

            cout << valuePrompt;
            getline(cin, input);

            if (!isNumeric(input)) {
                cerr << errorMessage << invalidNumberError << endl;
                continue;
            }

            try {
                v = stod(input);
            } catch (...) {
                cerr << errorMessage << convertNumberError << endl;
                continue;
            }

            listPtr[c].id = id;
            listPtr[c].value = v;
            c++;
        }
        if (c == 0) {
            cerr << noNumbersEnteredError << endl;
            return -1;
        }
    } else if (inputChoice == 'f') {
        string filename;
        while (true) {
            cout << inputFileNamePrompt;
            cin >> filename;
            inputFile.clear();
            inputFile.open(filename);
            if (!inputFile.is_open()) {
                cerr << errorMessage << openFileError << endl;
            } else
                break;
        }

        string line;
        while (getline(inputFile, line) && c < MAX_SIZE) {
            if (line.empty())
                continue;

            size_t colonPos = line.find(':');
            string toParse = (colonPos != string::npos) ? line.substr(colonPos + 1) : line;

            istringstream ss(toParse);
            int id;
            double value;

            if (ss >> id >> value) {
                listPtr[c].id = id;
                listPtr[c].value = value;
                c++;
            }
        }
        if (c == 0) {
            cerr << noNumbersInFileError << endl;
            inputFile.close();
            return -1;
        }
        inputFile.close();
    } else {
        cerr << errorMessage << endl;
        return -1;
    }
    return c;
}

// Do the calculation based on user's choice
double Toolkit::calculate() {
    double r = 0;
    switch (choice) {
    case ADD:
        op = '+';
        r = add(num1, num2);
        break;
    case SUBTRACT:
        op = '-';
        r = subtract(num1, num2);
        break;
    case MULTIPLY:
        op = '*';
        r = multiply(num1, num2);
        break;
    case DIVIDE:
        op = '/';
        r = divide(num1, num2);
        break;
    case SUM:
        op = 'S';
        r = sum(listPtr, count);
        break;
    case MEAN:
        op = 'A';
        r = mean(listPtr, count);
        break;
    case MEDIAN:
        op = 'D';
        r = median(listPtr, count);
        break;
    case MIN:
        op = 'm';
        r = min(listPtr, count);
        break;
    case MAX:
        op = 'X';
        r = max(listPtr, count);
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

            if (result < 0) {
                cout << lessThanMessage << endl;
            } else if (result > 0) {
                cout << greaterThanMessage << endl;
            } else {
                cout << equalToMessage << endl;
            }
            op = 'C';
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

            if (str1 < str2) {
                cout << lessThanMessage << endl;
            } else if (str1 > str2) {
                cout << greaterThanMessage << endl;
            } else {
                cout << equalToMessage << endl;
            }
            op = 'S';
        }
        break;

    case SAVE_LIST:
        {
            cout << saveListTitle << endl;

            listPtr = new DataItem[MAX_SIZE];

            cout << enterNumbersPrompt << endl;
            int id = 0;
            double v = 0;
            int c = 0;

            while (c < MAX_SIZE) {
                cout << idPrompt;
                string input;
                getline(cin, input);

                if (!isNumeric(input)) {
                    cerr << errorMessage << invalidIdError << endl;
                    continue;
                }

                try {
                    id = stoi(input);
                } catch (...) {
                    cerr << errorMessage << invalidIdError << endl;
                    continue;
                }

                if (id == -1)
                    break;

                cout << valuePrompt;
                getline(cin, input);

                if (!isNumeric(input)) {
                    cerr << errorMessage << invalidNumberError << endl;
                    continue;
                }

                try {
                    v = stod(input);
                } catch (...) {
                    cerr << errorMessage << convertNumberError << endl;
                    continue;
                }

                listPtr[c].id = id;
                listPtr[c].value = v;
                c++;
            }

            if (c > 0) {
                fstream dataFile;
                dataFile.open(dataFileName, ios::out | ios::binary);

                if (!dataFile) {
                    cerr << errorMessage << openFileError << endl;
                } else {
                    dataFile.write(reinterpret_cast<char*>(listPtr), c * sizeof(DataItem));
                    dataFile.close();
                    cout << listSavedMessage << endl;
                }
            } else {
                cerr << noNumbersEnteredError << endl;
            }

            delete[] listPtr;
            listPtr = nullptr;
            op = 'F';
        }
        break;

    case LOAD_LIST:
        {
            cout << loadListTitle << endl;

            fstream dataFile;
            dataFile.open(dataFileName, ios::in | ios::binary);

            if (!dataFile) {
                cerr << errorMessage << " " << noDataFileError << endl;
            } else {
                dataFile.seekg(0, ios::end);
                long fileSize = dataFile.tellg();

                int numRecords = fileSize / sizeof(DataItem);

                if (numRecords > 0) {
                    listPtr = new DataItem[numRecords];

                    dataFile.seekg(0, ios::beg);
                    dataFile.read(reinterpret_cast<char*>(listPtr), fileSize);

                    if (dataFile.fail()) {
                        cerr << errorMessage << openFileError << endl;
                    } else {
                        cout << statsHeader << endl;
                        cout << sumLabel << " = " << sum(listPtr, numRecords) << endl;
                        cout << meanLabel << " = " << mean(listPtr, numRecords) << endl;
                        cout << medianLabel << " = " << median(listPtr, numRecords) << endl;
                        cout << minLabel << " = " << min(listPtr, numRecords) << endl;
                        cout << maxLabel << " = " << max(listPtr, numRecords) << endl;
                    }

                    delete[] listPtr;
                    listPtr = nullptr;
                } else {
                    cerr << noNumbersInFileError << endl;
                }

                dataFile.close();
            }
            op = 'L';
        }
        break;

    case VIEW_RECORD:
        {
            cout << viewRecordTitle << endl;

            fstream dataFile;
            dataFile.open(dataFileName, ios::in | ios::binary);

            if (!dataFile) {
                cerr << errorMessage << " " << noDataFileError << endl;
            } else {
                cout << recordIndexPrompt;
                string input;
                getline(cin, input);

                if (!isNumeric(input)) {
                    cerr << errorMessage << invalidRecordError << endl;
                } else {
                    try {
                        int index = stoi(input);

                        dataFile.seekg(index * sizeof(DataItem), ios::beg);

                        DataItem temp;
                        dataFile.read(reinterpret_cast<char*>(&temp), sizeof(DataItem));

                        if (dataFile.fail()) {
                            cerr << errorMessage << " " << recordNotFoundError << endl;
                        } else {
                            cout << recordLabel << index << ": ID=" << temp.id
                                 << " Value=" << temp.value << endl;
                        }
                    } catch (...) {
                        cerr << errorMessage << invalidRecordError << endl;
                    }
                }

                dataFile.close();
            }
            op = 'V';
        }
        break;

    default:
        cout << invalidChoiceMessage << endl;
    }

    return r;
}

// Show the result on screen or save to file
int Toolkit::sendOutput() {
    string outLine;
    if (op == '+' || op == '-' || op == '*' || op == '/') {
        outLine = to_string(num1) + " " + op + " " + to_string(num2) + " = " + to_string(result);
    } else {
        string label;
        switch (op) {
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
        outLine = label + " = " + to_string(result);
    }

    if (outputChoice == 'c') {
        cout << outLine << endl;
    } else if (outputChoice == 'f') {
        string filename;
        cout << outputFileNamePrompt;
        cin >> filename;
        outputFile.open(filename);
        if (!outputFile) {
            cerr << errorMessage << endl;
            return -1;
        }
        outputFile << outLine << endl;
        outputFile.close();
    }
    return 0;
}

// Print list to console or file
void Toolkit::printList(const DataItem* list, int size, const string& title, const string& filePrompt) {
    if (outputChoice == 'c') {
        cout << "\n--- " << title << " ---" << endl;
        for (int i = 0; i < size; ++i) {
            cout << i << ": ID=" << list[i].id << " Value=" << list[i].value << endl;
        }
    } else if (outputChoice == 'f') {
        string filename;
        cout << filePrompt;
        cin >> filename;
        outputFile.open(filename);
        if (!outputFile) {
            cerr << errorMessage << endl;
            return;
        }
        for (int i = 0; i < size; ++i) {
            outputFile << i << ": ID=" << list[i].id << " Value=" << list[i].value << endl;
        }
        outputFile.close();
    }
}

// --- Math Operations ---

double Toolkit::add(double n1, double n2) {
    return n1 + n2;
}

double Toolkit::subtract(double n1, double n2) {
    return n1 - n2;
}

double Toolkit::multiply(double n1, double n2) {
    return n1 * n2;
}

double Toolkit::divide(double n1, double n2) {
    if (n2 == 0) {
        cout << divideByZeroError << endl;
        return 0;
    }
    return n1 / n2;
}

// --- Statistical Operations ---

double Toolkit::sum(const DataItem* list, int size) {
    double s = 0.0;
    for (int i = 0; i < size; ++i) {
        s += list[i].value;
    }
    return s;
}

double Toolkit::mean(const DataItem* list, int size) {
    if (size <= 0)
        return 0.0;
    return sum(list, size) / static_cast<double>(size);
}

double Toolkit::median(DataItem* list, int size) {
    if (size <= 0)
        return 0.0;

    double* tmp = new double[size];
    for (int i = 0; i < size; ++i) {
        tmp[i] = list[i].value;
    }
    sort(tmp, tmp + size);
    double result;
    if (size % 2 == 1) {
        result = tmp[size / 2];
    } else {
        double a = tmp[size / 2 - 1];
        double b = tmp[size / 2];
        result = (a + b) / 2.0;
    }
    delete[] tmp;
    return result;
}

double Toolkit::min(const DataItem* list, int size) {
    if (size <= 0)
        return 0.0;
    double m = list[0].value;
    for (int i = 1; i < size; ++i) {
        if (list[i].value < m)
            m = list[i].value;
    }
    return m;
}

double Toolkit::max(const DataItem* list, int size) {
    if (size <= 0)
        return 0.0;
    double m = list[0].value;
    for (int i = 1; i < size; ++i)
        if (list[i].value > m)
            m = list[i].value;
    return m;
}

// --- Sorting and Searching ---

void Toolkit::bubbleSort(DataItem* arr, int size) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (arr[j].value > arr[j + 1].value)
                swap(arr[j], arr[j + 1]);
        }
    }
    isSortedFlag = true;
}

void Toolkit::selectionSort(DataItem* arr, int size) {
    for (int i = 0; i < size - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < size; ++j)
            if (arr[j].value < arr[minIdx].value)
                minIdx = j;
        if (minIdx != i)
            swap(arr[i], arr[minIdx]);
    }
    isSortedFlag = true;
}

int Toolkit::linearSearch(const DataItem* arr, int size, double value) {
    for (int i = 0; i < size; ++i)
        if (arr[i].value == value)
            return i;
    return -1;
}

int Toolkit::binarySearch(const DataItem* arr, int size, double value) {
    int left = 0;
    int right = size - 1;
    while (left <= right) {
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

bool Toolkit::isSorted(const DataItem* arr, int size) {
    for (int i = 1; i < size; ++i) {
        if (arr[i - 1].value > arr[i].value)
            return false;
    }
    return true;
}

// --- Validation ---

bool Toolkit::isNumeric(string s) {
    if (s.empty())
        return false;

    bool hasDecimal = false;

    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];

        if (i == 0 && c == '-') {
            if (s.length() == 1)
                return false;
            continue;
        }

        if (c == '.') {
            if (hasDecimal)
                return false;
            hasDecimal = true;
            continue;
        }

        if (!isdigit(c))
            return false;
    }

    return true;
}
