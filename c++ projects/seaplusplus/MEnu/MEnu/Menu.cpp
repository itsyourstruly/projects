// Menu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

using std::cout;
using std::cin;

const char HELLO[] = "Hello\n";
const char BYE[] = "GoodBye\n";
const char SAYHI = '1';
const char SAYBYE = '2';
const char SPACE = ' ';
const char PROMPT[] = "Enter your choice: ";

int main() //Starting point for program
{
    //say hello or goodbye

    cout << SAYHI << SPACE << HELLO;
    cout << SAYBYE << SPACE << BYE;
    cout << PROMPT;
    char input;
    cin >> input;
    if (input == SAYHI || input == SAYBYE) {
        if (input == SAYHI)
        {
            cout << HELLO;
        }// if sayhi
        if (input == SAYBYE)
        {
            cout << BYE;
        }//if saybye
    }
    else {
        cout << "You did not enter 1 or 2.";
    }
    //if (input == SAYHI)
    //    {
    //        cout << HELLO;
    //    }// if sayhi
    //else
    //{
    //    cout << SAYBYE;
    //}//if else SAYHI

}//end of main

