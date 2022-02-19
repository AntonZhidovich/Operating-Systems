#include <iostream>
#include <fstream>
using namespace std;
struct employee{
	int num;
	char name[10];
	double hours;
	};

int main(int argc, char** argv){
	int n = atoi(argv[1]);
	employee* emps = new employee[n];
	for(int i = 0; i < n; i++){
		cout << "num name hours>";
		cin >> emps[i].num >> emps[i].name >> emps[i].hours;
		}

	ofstream out(argv[2], ios::binary);
	out.write((const char*)emps, sizeof(employee)*n);
	out.close();
	}