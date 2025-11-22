#include <iostream>
#include <string>
#include <ncurses.h>
#include <QCoreApplication>
#include <QStorageInfo>
#include <QDebug> // For printing output

using namespace std;

const char search[] = "Checking if you have a removable drive.";
const char notfound[] = "No removable drive found.";
const char found[] = "Removable drive found!";
const char prompt[] = "What would you like to do?";
const char open[] = "Open the drive";
const char eject[] = "Eject the drive";

void read()
{

}

int main()
{
    initscr(); // use ncurses
    cbreak();
    noecho();
    printw("%s", search);
    refresh();

}
