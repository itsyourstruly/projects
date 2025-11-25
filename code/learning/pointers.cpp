#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct cat {
    string name;
    int age;
    string color;
    string type;
    string gender;
};

void displayMessage(cat list[], int size);
void addCat(cat list[]);

int main()
{
    // const int SIZE = 3;
    vector<cat> catList[SIZE];
    // string input;

    catList[0] = {"Apollo", 1, "Black", "Bombay", "M"};
    catList[1] = {"Bonnie", 9, "Gray", "Domestic Short Hair", "F"};



    displayMessage(catList, SIZE);
    addCat(catList);

    return 0;

}


void displayMessage(cat list[], int size) {
    for (int i = 0; i < size; i++) {
        cout << "Cat: " << list[i].name
             << ", Age: " << list[i].age
             << ", Color: " << list[i].color
             << ", Type: " << list[i].type
             << ", Gender: " << list[i].gender << '\n';
    }
}
void addCat(cat list[]) {
    cout << "Lets add a new cat!\n";
    cout << "Enter name: ";
    getline(cin, list[2].name);
    cout << "Enter age: ";
    cin >> list[2].age;
    cout << "Enter color: ";
    cin.ignore();
    getline(cin, list[2].color);
    cout << "Enter Type: ";
    getline(cin, list[2].type);
    cout << "Enter Gender: ";
    getline(cin, list[2].gender);
}