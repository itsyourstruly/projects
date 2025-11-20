#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

//--Function prototypes
void swap(int& a, int& b);

void bubbleSort(int values[], int arrayLength);

void printArray(int arr[], int size);


int main() {
    int arraySize;
    
    // Seed the random number generator
    srand(time(0));
    
    // Ask user for the size of the array
    cout << "Enter the number of integers to sort: ";
    cin >> arraySize;
    
    // Create an array of the specified size
    int* testArray = new int[arraySize];
    
    // Fill the array with random integers (1-10000)
    for (int i = 0; i < arraySize; i++) {
        testArray[i] = rand() % 10000 + 1;
    }
    
    // Display the array before sorting
    cout << "Before increment: ";
    printArray(testArray, arraySize);
    
    // Perform bubble sort on the array
    bubbleSort(testArray, arraySize);
    
    // Display the array after sorting
    cout << "After increment: ";
    printArray(testArray, arraySize);
    
    // Clean up dynamically allocated memory
    delete[] testArray;
    
    return 0;
}

//--Function Definitions

// Function to swap two integers if the first is greater than the second
// Takes two integers by reference to modify the original values
void swap(int& a, int& b) {
    // Only swap if a is greater than b.
    if (a > b) {
        int temp = a;  // Store value of a in temporary variable
        a = b;         // Assign value of b to a
        b = temp;      // Assign original value of a (from temp) to b
    }
}

// Function to perform bubble sort on an integer array
// Takes an array and its length as parameters
void bubbleSort(int values[], int arrayLength) {
    // Outer loop: controls number of passes through the array
    // We need (arrayLength - 1) passes to ensure all elements are sorted
    for (int i = 0; i < arrayLength - 1; i++) {
        
        // Inner loop: compares adjacent elements in the array
        // After each pass, the largest element "bubbles up" to its correct position
        // So we can reduce the number of comparisons by i each time
        for (int j = 0; j < arrayLength - i - 1; j++) {
            
            // Compare adjacent elements and swap if they're in wrong order
            swap(values[j], values[j + 1]);
        }
    }
}

// Function to print array elements
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}
