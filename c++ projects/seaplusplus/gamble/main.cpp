#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>  // For sleep function on Linux/Unix

using namespace std;

const char welcomeMessage[] = "Welcome to the gambler's corner!";
const char instructionMessage[] = "Please enter your bet amount.";
const char moneyAmount[] = "Moolah: ";
const char betAmount[] = "Bet amount: ";
const char guessInput[] = "Guess the number (1-10): ";
const char invalidBetMessage[] = "Your too broke, bet something else!";
const char invalidGuessMessage[] = "Thats wrong, pick a number between 1 and 10!";
const char spinningMessage[] = "Spinning the wheel...";
const char randomNumberMessage[] = "The random number is: ";
const char correctGuessMessage[] = "You guessed correctly! Your new balance is: ";
const char wrongGuessMessage[] = "You guessed wrong! Your new balance is: ";
const char replayMessage[] = "Do you want to play again? y or q to quit";

const int quit = 'q';
const int playAgain = 'y';

int money;
int bet;
int guess;
bool playingGame;


void spin()
{
    cout << spinningMessage << endl;
    srand(time(0)); // Seed the random number generator
    sleep(1); // Wait for 1 second
    int randomNumber = rand() % 10 + 1; //choose number between 1 and 10
    cout << randomNumberMessage << randomNumber << endl;
    if (guess == randomNumber)
    {
        money += bet * 2; //double the bet if correct
        cout << correctGuessMessage << money << endl;
    }
    else
    {
        money -= bet; //lose the bet amount if wrong
        cout << wrongGuessMessage << money << endl;
    }
}

int main() 
{
    money = 100; //starting money
    cout << welcomeMessage << endl;

    playingGame = true;
    while (playingGame && money > 0)
    {
        cout << instructionMessage << endl;
        cout << moneyAmount << money << endl;
        cout << betAmount;
        cin >> bet;
        
        if (bet > money || bet <= 0)
        {
            cout << invalidBetMessage << endl;
            continue; // Skip to next iteration of loop
        }
        
        cout << guessInput;
        cin >> guess;
        if (guess < 1 || guess > 10)
        {
            cout << invalidGuessMessage << endl;
            continue; // Skip to next iteration of loop
        }
        
        spin();
        
        // Ask if player wants to continue
        cout << replayMessage << endl;
        char choice;
        cin >> choice;
        if (choice == quit)
        {
            cout << "Thanks for playing!" << endl;
            playingGame = false;
        }
        else if (choice != playAgain)
        {
            cout << "Invalid choice. Thanks for playing!" << endl;
            playingGame = false;
        }
    }
    
    if (money <= 0)
    {
        cout << "You're out of money! Game over!" << endl;
    }
    
    return 0;
}