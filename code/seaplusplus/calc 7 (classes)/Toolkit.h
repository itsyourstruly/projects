#ifndef TOOLKIT_H
#define TOOLKIT_H

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <limits>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

// Enum and Struct definitions
enum MenuChoice {
    ADD = 1, SUBTRACT, MULTIPLY, DIVIDE, SUM, MEAN, MEDIAN, MIN, MAX,
    BUBBLE_SORT, SELECTION_SORT, LINEAR_SEARCH, BINARY_SEARCH,
    C_STRING_COMPARE, STRING_COMPARE,
    SAVE_LIST, LOAD_LIST, VIEW_RECORD,
    EXIT
};

struct DataItem {
    int id;
    double value;
};

class Toolkit {

private:
    // --- Private Data Members ---
    MenuChoice choice;
    double num1, num2, result;
    char inputChoice, outputChoice, op, repeat;
    ifstream inputFile;
    ofstream outputFile;
    DataItem* listPtr;
    int count;
    bool isSortedFlag;

    // Constants
    static const int MAX_SIZE = 100;

    // --- Private Helper Functions ---
    // Math operations
    double add(double, double);
    double subtract(double, double);
    double multiply(double, double);
    double divide(double, double);

    // Statistical operations
    double sum(const DataItem*, int);
    double mean(const DataItem*, int);
    double median(DataItem*, int);
    double min(const DataItem*, int);
    double max(const DataItem*, int);

    // Sorting and searching
    void bubbleSort(DataItem*, int);
    void selectionSort(DataItem*, int);
    int linearSearch(const DataItem*, int, double);
    int binarySearch(const DataItem*, int, double);
    bool isSorted(const DataItem*, int);

    // Input/Output helpers
    int getListInput();
    void printList(const DataItem*, int, const string&, const string&);

    // Validation
    bool isNumeric(string);

public:
    // --- Constructor ---
    Toolkit();

    // --- Public Interface ---
    void run();
    void displayMenu();
    void selectIO();
    int getInput();
    double calculate();
    int sendOutput();
};

#endif // TOOLKIT_H