#include <iostream>

void math(const int *num, int size);

int main()
{
    const int SIZE = 5;
    const int numbers[SIZE]  {10, 20, 30, 40, 50};
    int morenumbers[SIZE] {5,10,15,20,25};

    math(numbers, SIZE);
    math(morenumbers, SIZE);


}

void math(const int *num, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << *(num + i) << " ";
    }
    std::cout << '\n';
}
