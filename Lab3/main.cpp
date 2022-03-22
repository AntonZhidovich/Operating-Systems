#include <iostream>
#include <time.h>
#include <windows.h>
CRITICAL_SECTION cs;
int main() {
    int n;
    srand(time(0));
    n = 10 + rand() % 21; //from 10 to 30 elems
    int* arr = new int[n];
    for(int i = 0; i < n; i++){
        arr[i] = 0;
    }
    int threadCount = 3 + rand() % 6; // from 3 to 8 threads

    HANDLE startThreadsEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&cs);
    DeleteCriticalSection(&cs);
    return 0;
}
