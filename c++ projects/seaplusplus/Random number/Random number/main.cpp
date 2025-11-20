//
//  main.cpp
//  Random number
//
//  Created by Matthew Nakhel on 5/15/25.
//

#include <iostream>
using std::cout;
using std::cin;

const char PROMPT[] = "Please enter a number between ";
const char QUITPROMPT[] = "Enter Q to quit.";
const char GOODJOB[] = "Good Job!";
const char AND[] = "and";
const char min = '1';
const char max = '9';
const char quit = 'Q';
const char GOODINPUT[] = "That is Correct";
const char BADINPUT[] = "That is not Correct";
const char nl = '\n';



int main(int argc, const char * argv[]) {
    
    char input = 'x';
    int random = 0;

    
    // Calculate random number between the min and max
    
    random = rand() % (max - min + 1) + min;
    
    while (input != quit)
    {
        cout << PROMPT << min << AND << max << QUITPROMPT;
        cin >> input;
        input = input - '0';
        
        if (input == random)
        {
            cout << GOODJOB << nl;
        }
        else
        {
            cout << BADINPUT << nl;
        }
    }
    

}
