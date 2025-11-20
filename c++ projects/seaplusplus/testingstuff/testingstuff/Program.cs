// AnimalMenu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

using std::cout;
using std::cin;

const char DOG[] = "Dog\n";
const char BIRD[] = "Bird\n";
const char CAT[] = "Cat\n";
const char DINOSAUR[] = "Dinosaur\n";
const char SAYDOG = '1';
const char SAYBIRD = '2';
const char SAYCAT = '3';
const char SAYDINOSAUR = '4';
const char SPACE = ' ';
const char PROMPT[] = "Pick an animal from the following options.\n";

int main()
{
    cout << PROMPT;
    cout << SAYDOG << SPACE << DOG;
    cout << SAYBIRD << SPACE << BIRD;
    cout << SAYCAT << SPACE << CAT;
    cout << SAYDINOSAUR << SPACE << DINOSAUR;
    char input;
    cin >> input;
    if (input == SAYDOG || input == SAYBIRD || input == SAYCAT || input == SAYDINOSAUR) {
        if (input == SAYDOG)
        {
            cout << DOG;
        }//if user picks dog
        if (input == SAYBIRD)
        {
            cout << BIRD;
        }//if user picks bird
        if (input == SAYCAT)
        {
            cout << CAT;
        }//if user picks cat
        if (input == SAYDINOSAUR)
        {
            cout << DINOSAUR;
        }//if user picks dinosaur
    }
    else {
        cout << "You had one job."; 
    }//if user types the wrong option
}

