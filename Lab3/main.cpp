#include <iostream>
#include <time.h>
#include <vector>
#include <windows.h>
#include <process.h>
using std::cin;
const int SLEEP_TIME = 5;
CRITICAL_SECTION cs;
HANDLE startThreadsEvent;
std::vector<HANDLE> threadEvents;

struct threadArgs{
    int* arr;
    int n;
    int num;
    HANDLE actions[2];
    threadArgs(int* _arr, int _n, int _num) : arr(_arr), num(_num),  n(_n){
        //Second action is stop thread
        actions[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
        actions[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
};

void printArray(int* arr, int n){
    EnterCriticalSection(&cs);
    for(int i = 0; i < n; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
    LeaveCriticalSection(&cs);
}

UINT WINAPI marker(void *p){
    threadArgs* args = static_cast<threadArgs*>(p);
    WaitForSingleObject(startThreadsEvent, INFINITE);
    srand(args->num);
    printf("Thread #%d started.\n", args->num);

    int count = 0;
    while(true){
        EnterCriticalSection(&cs);
        int i = rand() % args->n;
        if(0 == args->arr[i]){
            Sleep(SLEEP_TIME);
            args->arr[i] = args->num;
            ++count;
            LeaveCriticalSection(&cs);
            Sleep(SLEEP_TIME);
        } else {
            printf("Thread #%d. Marked %d elems. Unable to mark a[%d].\n", args->num, count, i);
            LeaveCriticalSection(&cs);
            SetEvent(threadEvents[args->num-1]);
            int action = WaitForMultipleObjects(2,args->actions, FALSE, INFINITE) - WAIT_OBJECT_0;
            if(1 == action){
                for(int i = 0; i < args->n; i++){
                    if(args->arr[i] == args->num){
                        args->arr[i] = 0;
                    }
                }
                printf("Thread #%d is terminated.\n", args->num);
                break;
            }
        }
    }
    delete args;
    return NULL;
}

int main() {
    //creating array
    int n;
    srand(time(0));
    n = 10 + rand() % 21; //from 10 to 30 elems
    int* arr = new int[n];
    for(int i = 0; i < n; i++){
        arr[i] = 0;
    }
    printf("Array of %d elements is created.\n", n);

    //creating threads
    int threadCount = 3 + rand() % 6; // from 3 to 8 threads
    startThreadsEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    std::vector<HANDLE> threads;
    HANDLE currThread;
    std::vector<threadArgs*> argsVec;
    threadArgs* currArgs;
    bool* terminated = new bool[threadCount];
    for(int i = 0; i < threadCount; i++){
        currArgs = new threadArgs(arr, n, i+1);
        currThread =
                (HANDLE)_beginthreadex(NULL, 0, marker, currArgs, 0, NULL);
        if(currThread == NULL) {
            printf("Thread can't be created.\n");
            return GetLastError();
        }
        terminated[i] = false;
        threadEvents.push_back(CreateEvent(NULL, TRUE, FALSE, NULL));
        argsVec.push_back(currArgs);
        threads.push_back(currThread);
    }
    printf("%d threads are ready to start.\n" , threadCount);

    //starting threads
    InitializeCriticalSection(&cs);
    PulseEvent(startThreadsEvent);
    int terminatedCount = 0, k;
    while(terminatedCount != threadCount) {
        WaitForMultipleObjects(threadCount, &threadEvents[0], TRUE, INFINITE);
        printArray(arr, n);
        printf("All threads are paused. Which one is to terminate?\n");
        cin >> k;
        if(k <= 0 || k > threadCount || terminated[k - 1]){
            printf("Invalid index. Try again.\n");
            continue;
        }
        terminated[k-1] = true;
        SetEvent(argsVec[k - 1]->actions[1]);
        WaitForSingleObject(threads[k-1], INFINITE);
        ++terminatedCount;
        printArray(arr, n);
        for(int i = 0; i < threadCount; ++i){
            if(terminated[i])
                continue;
            ResetEvent(threadEvents[i]);
            SetEvent(argsVec[i]->actions[0]);
        }
    }
    DeleteCriticalSection(&cs);
    delete[] terminated;
    delete[] arr;
    return 0;
}
