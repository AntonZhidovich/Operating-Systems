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

void marker(int num){
    std::unique_lock<std::mutex> startlk(startMutex);
    startCV.wait(startlk, []()->bool{return ready;});
    srand(num);
    printf("Thread #%d is started.\n", num);
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
    for(int i = 0; i < threadCount; i++){
        threads.push_back(std::thread(marker, i+1));
        threads.back().detach();
    }
    ready = true;
    startCV.notify_all();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}