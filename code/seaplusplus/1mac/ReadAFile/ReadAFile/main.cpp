#include <iostream>
#include <string>
#include <fstream> // For file input and output
#include <sstream> // For stringstream to split lines
using namespace std;

const char ERROR[] = "Unable to open file";
const char File[] = "Names.csv";
const char comma = ',';

int main() {
    string line; // Create a string to store the names in the file
    ifstream myfile(File); // Opens the file

    if (myfile.is_open()) {
        while (getline(myfile, line)) { // Read each line from the file and store it in the string line
            stringstream ss(line); // Create a stringstream for the string line
            string value; // create a string to store each name

            while (getline(ss, value, comma)) { // Read each name separated by commas
                cout << value << endl; // Print each value and add a new line
            }
        }
        myfile.close(); // Close the file after reading
    } else {
        cout << ERROR; // Error message if the file cannot be opened
    }

    return 0;
}
