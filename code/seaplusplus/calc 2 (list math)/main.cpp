#include <fstream>
#include <iostream>
#include <limits>
#include <cctype>
#include <algorithm>

using namespace std;

const char menuTitle[] = "Calculator Menu";
const char menuOptions[] = "1. Add\n2. Subtract\n3. Multiply\n4. Divide\n";
const char menuPrompt[] = "Please input an option from the above menu (1-4)";
const char repeatPrompt[] = "Would you like to perform another calculation? (Y/N): ";
const char inputMethods[] = "Select input method - keyboard (k) or file (f): ";
const char outputMethods[] = "Select output method - console (c) or file (f): ";
const char firstNumberPrompt[] = "Enter first number: ";
const char secondNumberPrompt[] = "Enter second number: ";
const char inputFileNamePrompt[] = "Enter input file name: ";
const char outputFileNamePrompt[] = "Enter output file name: ";
const char errorMessage[] = "Error, please try again.";

// Function prototypes
void displayMenu(int &);
void selectIO(char &, char &);
int getInput(char &, double &, double &, ifstream &);
double calculate(int &, char &, double &, double &);
int sendOutput(char &, double &, char &, double &, double &, ofstream &);

// basic calculations
double add(double, double);
double subtract(double, double);
double multiply(double, double);
double divide(double, double);
double sum(const double list[], int size);
double mean(const double list[], int size);
double median(double list[], int size); // Requires data to be sorted in place or copied.
double min(const double list[], int size);
double max(const double list[], int size);

// Function definitions
// main
int main()
{
    int choice; // Use int for menu choice
    double num1 = 0, num2 = 0, result = 0;
    char inputChoice, outputChoice, op, repeat;
    ifstream inputFile;
    ofstream outputFile;
    vector<double> numbers; // Vector to hold list of numbers for advanced calculations

    do
    {
        displayMenu(choice); // calls displayMenu function and passes user choice
        selectIO(inputChoice, outputChoice); // calls selectIO function to pass input and output chocies
        getInput(inputChoice, num1, num2, inputFile); // calls getInput function to get numbers
        result = calculate(choice, op, num1, num2); // calls calculation functions
        sendOutput(outputChoice, num1, op, num2, result, outputFile); // calls sendOutput function to display result

        cout << endl << repeatPrompt;
        cin >> repeat;
        // TODO: Add input validation for the repeat variable.
    } while ((tolower(repeat) != 'n')); // Loop will be controlled by user input

    return 0;
} // main

// Display the user menu and pass choice
void displayMenu(int &c)
{
    while (c < 1 || c > 4) // validate choice, if user inputs number less than 1 or greater than 4 it reprompts
    {
        cout << menuTitle << endl;
        cout << menuOptions << endl;
        cout << menuPrompt << endl;
        cin >> c;
    } // displays menu options and gets user choice
}

// let user select input and output options
void selectIO(char &i, char &o)
{
    while (i != 'k' && i != 'f') // validate input choice, if user inputs other than k or f it reprompts
    {
        cout << inputMethods; // display prompt
        cin >> i; // get user input
    }
    while (o != 'c' && o != 'f') // validate output choice, if user inputs other than c or f it reprompts
    {
        cout << outputMethods;
        cin >> o;
    }
} 

// get input values from keyboard or file
int getInput(char &i, double &n1, double &n2, ifstream &f)
{
    if (i == 'k') // if input choice is keyboard
    {
        cout << firstNumberPrompt;
        cin >> n1;
        cout << secondNumberPrompt;
        cin >> n2;
    }
    else if (i == 'f') // if input choice is file
    {
        string filename;
        ifstream f;
        while (true) // loop to prompt for filename until a valid file is opened
        {
            cout << inputFileNamePrompt;
            cin >> filename;
            f.open(filename);
            if (!f.is_open()) // check if file did not open, if so display error and reprompt
            {
                cerr << errorMessage << endl;
            }
            else
            {
                break; // Exit loop if file opens successfully
            }
        } 
        f >> n1 >> n2;
        f.close();
    }
    else
    {
        cerr << errorMessage << endl;
        return -1; // Indicate error
    }
    return 0;
}

// calculate result
double calculate(int &c, char &op, double &n1, double &n2)
{
    double r = 0;
    // Perform calculation
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
    case 5:  // sum of a list
        op = '+';
    case 6:  // mean of a list
    case 7:  // median of a list
    case 8:  // min of a list
    case 9:  // max of a list
    default: // invalid
        cout << "\n *** Invalid choice. ***" << endl;
    }

    return r;
} // calculate

// send output to console or file
int sendOutput(char &o, double &n1, char &op, double &n2, double &r, ofstream &f)
{
    if (o == 'c')
    {
        cout << n1 << " " << op << " " << n2 << " = " << r << endl;
    }
    else if (o == 'f')
    {
        string filename;
        cout << outputFileNamePrompt;
        cin >> filename;
        f.open(filename);
        f << n1 << " " << op << " " << n2 << " = " << r << endl;
        if (!f)
        {
            cerr << errorMessage << endl;
            return -1; // returns error if file cannot be opened
        }
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
    if (num1 == 0 || num2 == 0) // check for division by zero
    {
        cout << "cannot divide by zero" << endl;
        return 0; 
    }
    else
    {
        return num1 / num2;
    }
} // divide
double sum(const double list[], int size)
{
    return 0;
} // sum
double mean(const double list[], int size)
{
    return 0;
} // mean
double median(double list[], int size)
{
    return 0;
} // median
double min(const double list[], int size)
{
    return 0;
} // min
double max(const double list[], int size)
{
    return 0;
} // max