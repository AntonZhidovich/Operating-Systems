#include <iostream>
#include <ctime>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

const int SLEEP_TIME = 5;

std::condition_variable startCV;
std::mutex startMutex;
bool ready;
std::mutex markerMutex;
bool *threadsPaused;
std::condition_variable stateChanged;
std::mutex stateMutex;

struct threadArgs{
    int* arr;
    int n;
    int num;
    bool actions[2];
    //[0] - to continue
    //[1] - to terminate
    std::condition_variable doAction;
    std::mutex actionMutex;
    threadArgs(int* _arr, int _n, int _num) : arr(_arr), num(_num),  n(_n){
        actions[0] = false;
        actions[1] = false;
    }
};

bool allPaused(bool* arr, int n){
    for(int i = 0; i < n; i++){
        if(!arr[i])
            return false;
    }
    return true;
}

void marker(threadArgs* args){
    std::unique_lock<std::mutex> startlk(startMutex);
    startCV.wait(startlk, []()->bool{return ready;});
    startlk.unlock();
    printf("Thread #%d is started.\n", args->num);
    int count = 0;
    srand(args->num);
    while (true) {
        std::unique_lock<std::mutex> lk(markerMutex);
        int i = rand() % args->n;
        if (args->arr[i] == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
            args->arr[i] = args->num;
            ++count;
            lk.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        } else{
            printf("Thread #%d. Marked %d elems. Unable to mark a[%d].\n", args->num, count, i);
            lk.unlock();
            threadsPaused[args->num - 1] = true;
            stateChanged.notify_all();
            std::unique_lock<std::mutex> actionLock(args->actionMutex);
            args->doAction.wait(actionLock, [=]()->bool{return args->actions[1] || args->actions[0];});
            if(args->actions[1]){
                for(int i = 0; i < args->n; i++) {
                    if (args->arr[i] == args->num) {
                        args->arr[i] = 0;
                    }
                }
                printf("Thread #%d is terminated.\n", args->num);
                break;
            }
            args->actions[0] = false;
        }
    }
}

void printArray(int* arr, int n){
    std::lock_guard<std::mutex> lk(markerMutex);
    for(int i = 0; i < n; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(){
    //generating an array
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
    std::vector<std::thread> threads;
    std::vector<threadArgs*> argsVec;
    threadsPaused = new bool[threadCount];
    bool* terminated = new bool[threadCount];
    for(int i = 0; i < threadCount; i++){
        argsVec.push_back(new threadArgs(arr, n, i+1));
        threads.push_back(std::thread(marker, argsVec.back()));
        terminated[i] = false;
    }
    printf("%d threads are ready to start.\n" , threadCount);
    //starting threads
    ready = true;
    startCV.notify_all();

    int terminatedCount = 0;
    int k;
    while(terminatedCount != threadCount){
        std::unique_lock<std::mutex> lk(stateMutex);
        stateChanged.wait(lk, [=](){return allPaused(threadsPaused, threadCount);});
        printArray(arr, n);
        printf("All threads are paused. Which one is to terminate?\n");
        std::cin >> k;
        if(k <= 0 || k > threadCount || terminated[k - 1]){
            printf("Invalid index. Try again.\n");
            continue;
        }
        terminated[k-1] = true;
        argsVec[k-1]->actions[1] = true;
        argsVec[k-1]->doAction.notify_one();
        threads[k-1].join();
        ++terminatedCount;
        printArray(arr, n);
        printf("\n");
        for(int i = 0; i < threadCount; ++i){
            if(terminated[i])
                continue;
            threadsPaused[i] = false;
            argsVec[i]->actions[0] = true;
            argsVec[i]->doAction.notify_one();
        }
    }
    printf("All threads are terminated.\n");
    for(auto a: argsVec){
        delete a;
    }
    delete[] threadsPaused;
    delete[] terminated;
    delete[] arr;
    return 0;
}