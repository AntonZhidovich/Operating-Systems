#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

struct employee{
	int num;
	char name[10];
	double hours;
	};

int main(int argc, char** argv){
	char* binName = argv[1];
	char* reportName = argv[2];
	int salary = atoi(argv[3]);
	ifstream in(binName, ios::in|ios::binary|ios::ate);
	in.seekg(0, ios::end);
	int n = in.tellg()/sizeof(employee);
	in.seekg(0, ios::beg);
	employee* emps = new employee[n];
	in.read((char*)emps, n*sizeof(employee));

	ofstream out(reportName);
	out << binName << endl;	
	out << left;
	out  << std::setw(20) << "Номер сотрудника " 
		<< std::setw(20) << "имя сотрудника " 
		<< std::setw(10) <<  "часы " 
		 << "зарплата\n";
	for(int i = 0; i < n; i++){
		out << std::setw(20) << emps[i].num  
			<< std::setw(20) << emps[i].name
			<< std::setw(10)<< emps[i].hours 
			<< std::setw(10) << emps[i].hours*salary << endl;
		}
	out.close();
	}