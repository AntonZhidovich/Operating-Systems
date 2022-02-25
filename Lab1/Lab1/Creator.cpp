#include <iostream>
#include <fstream>
using std::ofstream;
using std::ios;

struct employee{
	int num;
	char name[10];
	double hours;
	};

int main(int argc, char** argv){
	int n = atoi(argv[1]);
	employee* emps = new employee[n];
	for(int i = 0; i < n; i++){
		std::cout << "num name hours>";
		std::cin >> emps[i].num >> emps[i].name >> emps[i].hours;
		}
	try{
	ofstream out(argv[2], ios::binary);
	out.write((const char*)emps, sizeof(employee)*n);
	out.close();
		}	
	catch(std::exception e){
		std::cerr << e.what() << std::endl;
		}
	}