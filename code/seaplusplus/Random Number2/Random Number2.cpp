#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

using std::cout;
using std::cin;

const char PROMPT[] = "Please enter a number between ";
const char QUITPROMPT[] = " Enter Q to quit.";
const char GOODJOB[] = "Good Job!";
const char AND[] = " and ";
const int min = 1;
const int max = 10;
const char quit = 'Q';
const char quit2 = 'q';
const char BADINPUT[] = "That is not correct.";
const char REPLAYPROMPT[] = "Do you want to play again? (Input anything for a rematch or Q to quit.)";
const char nl = '\n';

int main()
{
    char input = 'x';
    int random = 0;
    char play = 'y';
    
    while (play == 'y')
    {
        //calculate random number between min and max
        srand(static_cast<unsigned int>(time(NULL)));
        random = min + std::rand() % (max - min + 1); //% is modules and gets you the remainder.
        
        while (play != quit && play != quit2)
        {
            
            cout << PROMPT << min << AND << max << nl;
            cin >> input;

            input = input - '0'; //convert input to int
            
            if (input < min || input > max)
            {
                cout << BADINPUT << nl;
            } // if a number above 10 or below 1 is entered
            else if (input == random)
            {
                cout << GOODJOB << nl;
                cout << REPLAYPROMPT;
                cin >> play;
                play = 'y';
            } // if the answer is correct
            else
            {
                if (input < random)
                {
                    cout << "Too low, please try again\n";
                } // guess was too low
                else
                {
                    cout << "Too high, please try again\n";
                } // guess was too high
            }
        } //end play != quit
        
    } //end play == 'y'
    return 0;

}
