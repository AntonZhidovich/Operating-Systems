#include <iostream>
#include <windows.h>
#include <process.h>
using std::cout;
using std::cin;
using std::endl;

struct Args {
    int* arr;
    int size;
    int maxInd;
    int minInd;
    int avg;
};

UINT WINAPI min_max(void* p){
    Args* args = static_cast<Args*>(p);
    int n = args->size;
    int* arr = args->arr;
    int maxInd, minInd;
    maxInd = minInd = 0;
    for(int i = 1; i < n; i++) {
        if(arr[maxInd] < arr[i]){
            maxInd = i;
        }
        if(arr[minInd] > arr[i]){
            minInd = i;
        }
        Sleep(7);
    }
    args->maxInd = maxInd;
    args->minInd = minInd;
    cout << "Max element: " << arr[maxInd] << endl;
    cout << "Min element: " << arr[minInd] << endl;
    return 0;
}

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
    printf("\n\n");

    Args* args = new Args();
    args->size = n;
    args->arr = arr;

    HANDLE hmin_max;
    hmin_max = (HANDLE)
            _beginthreadex(NULL, 0, min_max, args, 0, NULL);
    if(hmin_max == NULL) {
        return GetLastError();
    }

    WaitForSingleObject(hmin_max, INFINITE);
    return 0;
}
