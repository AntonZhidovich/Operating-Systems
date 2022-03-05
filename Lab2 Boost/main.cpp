#include <iostream>
#include <conio.h>
#include <boost/thread.hpp>
#include <boost/random.hpp>
#include <boost/chrono.hpp>

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
	_getch();
	return 0;
}