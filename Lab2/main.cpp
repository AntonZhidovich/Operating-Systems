#include <iostream>
#include <windows.h>

using std::cout;
using std::cin;
using std::endl;

void printArr(int* arr, int n){
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
}

int main() {
    srand(time(0));
    int n = 5 + rand() % 15;
    int* arr = new int[n];
    for(int i = 0; i < n; i++){
        arr[i] = rand();
    }
    printf("Generated array of %d elements: \n", n);
    printArr(arr, n);

    return 0;
}
