#include <iostream>
#include <time.h>
#include <vector>
#include <windows.h>
#include <process.h>
using std::cout;
using std::cin;
using std::endl;
using std::flush;

CRITICAL_SECTION cs;
HANDLE startThreadsEvent;

UINT WINAPI threadFunc(void *p){
    WaitForSingleObject(startThreadsEvent, INFINITE);
    cout << "Thread started.\n" << flush;
    return NULL;
}

int main() {
    int n;
    srand(time(0));
    n = 10 + rand() % 21; //from 10 to 30 elems
    int* arr = new int[n];
    for(int i = 0; i < n; i++){
        arr[i] = 0;
    }
    printf("Array of %d elements is created.\n", n);

    int threadCount = 3 + rand() % 6; // from 3 to 8 threads
    startThreadsEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    std::vector<HANDLE> threads;
    HANDLE currThread;
    for(int i = 0; i < threadCount; i++){
        currThread =
                (HANDLE)_beginthreadex(NULL, 0, threadFunc, NULL, 0, NULL);
        if(currThread == NULL){
            cout << "Thread can't be created." << endl;
            return GetLastError();
        }
        threads.push_back(currThread);
    }
    printf("%d threads are ready to start.\n" , threadCount);

    SetEvent(startThreadsEvent);
    WaitForMultipleObjects(threadCount, &threads[0], TRUE, INFINITE);
    InitializeCriticalSection(&cs);
    DeleteCriticalSection(&cs);
    return 0;
}
