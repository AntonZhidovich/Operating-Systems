#include <iostream>
#include <ctime>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

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
    threadArgs(int* _arr, int _n, int _num) : arr(_arr), num(_num),  n(_n){
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
    srand(args->num);
    printf("Thread #%d is started.\n", args->num);
    int count = 0;

    while (true) {
        std::unique_lock<std::mutex> lk(markerMutex);
        int i = rand() % args->n;
        if (args->arr[i] == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            args->arr[i] = args->num;
            ++count;
            lk.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        } else{
            printf("Thread #%d. Marked %d elems. Unable to mark a[%d].\n", args->num, count, i);
            lk.unlock();
            threadsPaused[args->num - 1] = true;
            stateChanged.notify_all();
            break;
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
    std::cout << threadCount << std::endl;
    std::vector<std::thread> threads(threadCount);
    std::vector<threadArgs*> argsVec(threadCount);
    threadsPaused = new bool[threadCount];
    for(int i = 0; i < threadCount; i++){
        argsVec.push_back(new threadArgs(arr, n, i+1));
        threads.push_back(std::thread(marker, argsVec.back()));
        threads.back().detach();
    }
    ready = true;
    startCV.notify_all();

    int terminatedCount = 0;
    while(terminatedCount != threadCount){
        std::unique_lock<std::mutex> lk(stateMutex);
        stateChanged.wait(lk, [=](){return allPaused(threadsPaused, threadCount);});
        //printArray(arr, n);
        break;
    }
    return 0;
}