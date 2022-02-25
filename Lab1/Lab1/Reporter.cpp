#include <iostream>
#include <iomanip>
#include <fstream>
#include <conio.h>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::left;

struct employee{
	int num;
	char name[10];
	double hours;
	};

int compare(const void* p1, const void* p2){
	employee* e1 = (employee*)p1;
	employee* e2 = (employee*)p2;
	return e1->num - e2->num;
	}

int main(int argc, char** argv){
	char* binName = argv[1];
	char* reportName = argv[2];
	int salary = atoi(argv[3]);
	int n;
	employee* emps;
	ifstream in(binName, ios::in|ios::binary|ios::ate);
	if(!in.is_open()) {
		std::cerr << "File is not open." << std::endl;
		return 0;
		}
	in.seekg(0, ios::end);
	n = in.tellg()/sizeof(employee);
	in.seekg(0, ios::beg);
	emps = new employee[n];
	in.read((char*)emps, n*sizeof(employee));

	qsort(emps, n, sizeof(employee), compare);
	try{
	ofstream out(reportName);
	out << binName << endl;	
	out << left;
	out  << std::setw(20) << "Number " 
		<< std::setw(20) << "Name " 
		<< std::setw(10) <<  "Hours " 
		 << "Salary\n";
	for(int i = 0; i < n; i++){
		out << std::setw(20) << emps[i].num  
			<< std::setw(20) << emps[i].name
			<< std::setw(10)<< emps[i].hours 
			<< std::setw(10) << emps[i].hours*salary << endl;
		}
	out.close();
		}
	catch(std::exception e){
		std::cerr << e.what() << std::endl;
		getch();
		return 0;
		}
	}