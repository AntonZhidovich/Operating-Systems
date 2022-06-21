#include <iostream>
#include <conio.h>
#include <boost/thread.hpp>
#include <boost/random.hpp>
#include <boost/chrono.hpp>

const int SLEEP_TIME1 = 7;
const int SLEEP_TIME2 = 12;

struct Args {
	int* arr;
	int size;
	int maxInd;
	int minInd;
	int avg;
};

void min_max(Args* args) {
	int n = args->size;
	int* arr = args->arr;
	int maxInd, minInd;
	maxInd = minInd = 0;
	for (int i = 1; i < n; i++) {
		if (arr[maxInd] < arr[i]) {
			maxInd = i;
		}
		if (arr[minInd] > arr[i]) {
			minInd = i;
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIME1));
	}
	args->maxInd = maxInd;
	args->minInd = minInd;
	std::cout << "Max element: " << arr[maxInd] << std::endl;
	std::cout << "Min element: " << arr[minInd] << std::endl;
}

void average(Args* args) {
	int n = args->size;
	int* arr = args->arr;
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += arr[i];
		boost::this_thread::sleep_for(boost::chrono::milliseconds(SLEEP_TIME2));
	}
	args->avg = sum / n;
	printf("Average value: %d \n", args->avg);
}

void printArr(int* arr, int n) {
	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
}

int main() {

	//array generation
	typedef boost::mt19937 RNGType;
	RNGType rng(time(0));
	boost::uniform_int<> arraySize(5, 15);
	boost::variate_generator<RNGType, boost::uniform_int<>> dice(rng, arraySize);
	int n = dice();
	boost::uniform_int<> arrayElem(0, 35000);
	dice.distribution() = arrayElem;
	int* arr = new int[n];
	for (int i = 0; i < n; i++) {
		arr[i] = dice();
	}
	printf("Generated array of %d elements: \n", n);
	printArr(arr, n);
	printf("\n\n");

	//Init args 
	Args* args = new Args();
	args->size = n;
	args->arr = arr;

	//create threads
	boost::thread min_maxThread(min_max, args);
	boost::thread averageThread(average, args);
	min_maxThread.join();
	averageThread.join();

	arr[args->minInd] = arr[args->maxInd] = args->avg;
	std::cout << "\nChanged array: " << std::endl;
	printArr(arr, n);
	_getch();
	delete args;
	delete[] arr;
	return 0;
}