#include <iostream>
#include <fstream>
#include <conio.h>
#include <iomanip>
#include <string>
#include <windows.h>

using std::cin;
using std::cout;
using std::endl;
using std::ios;
using std::string;
using std::left;
using std::ifstream;

const int FILENAME_SIZE = 20;
const int ARGS_SIZE = 50;

struct employee{
	int num;
	char name[10];
	double hours;
	};

void printToConsole(char filename[FILENAME_SIZE]){
	ifstream in(filename, ios::in|ios::binary|ios::ate);
	in.seekg(0, ios::end);
	int n = in.tellg()/sizeof(employee);
	in.seekg(0, ios::beg);
	employee* emps = new employee[n];
	in.read((char*)emps, n*sizeof(employee));
	in.close();
	cout << left;
	for(int i = 0; i < n; i++){
		cout << std::setw(20) << emps[i].num  
			<< std::setw(20) << emps[i].name
			<< std::setw(10)<< emps[i].hours << endl;
		}
}

bool createBin(int count, char filename[]){
	STARTUPINFO si;
	PROCESS_INFORMATION piApp;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	char args[50] = "Creator.exe ";
	char buff[10];
	strcat(args, itoa(count, buff, 10 ));
	strcat(args, " ");
	strcat(args, filename);
	bool isStarted = CreateProcess(NULL, args, NULL, NULL, FALSE,
		 CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp);
	WaitForSingleObject(piApp.hProcess, INFINITE); 
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);
	return isStarted;
	}

bool writeReport(char filename[], char reportname[], int salary){
	STARTUPINFO si;
	PROCESS_INFORMATION piApp;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	char args[ARGS_SIZE] = "Reporter.exe ";
	char buff[10];
	strcat(args, filename); strcat(args, " ");
	strcat(args, reportname); strcat(args, " ");
	strcat(args, itoa(salary, buff, 10));
	bool isStarted = CreateProcess(NULL, args, NULL, NULL, FALSE,
		 CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp);
	WaitForSingleObject(piApp.hProcess, INFINITE); 
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);
	return isStarted;
	}

int main(int argc, char *argv[]){
	setlocale(LC_ALL, "Russian");
	char lpszAppName[] = "";
	cout << "number of records\n>";
	int count; cin >> count;
	cout << "filename\n>";
	char filename[FILENAME_SIZE]; cin >> filename;
	if(createBin(count, filename)){
		cout << "Data file created." << endl;
		printToConsole(filename);
		}
	else{
		cout << "Data file was not created." << endl;
		}
	cout << "\n\nName of the report file: \n>";
	char reportname[FILENAME_SIZE];
	cin >> reportname;
	cout << "Salary: \n>";
	int salary; cin >> salary;
	if(writeReport(filename, reportname, salary)){
		cout << "Report file created." << endl;
		ifstream in(reportname);
		string line;
		while(getline(in, line)){
			cout << line << endl;
			}
		}
	else{
		cout << "Report file was not created." << endl;
		}
	system("pause");
	return 0;
}
