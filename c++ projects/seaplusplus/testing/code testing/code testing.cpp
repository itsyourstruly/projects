
//Copy this into your IDE(Integrated Development Environment) and fix it there. It wil be easier and you can test it.

// FixThisWeek02.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
int main()
{
    std::cout << "Hello World!\n";
    std::cout << "Please input a letter:";
    char x;
    std::cin >> x;
    if (x == 'x' || x == 'A') {
        if (x == 'x') {
        std::cout << "You entered the letter x" << std::endl;
        }
        if (x == 'A') {
            std::cout << x << " is better than x" << std::endl;
        }
    }
    else {
        std::cout << "You entered: " << x << std::endl;
    }
}

