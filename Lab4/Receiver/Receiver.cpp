#include <iostream>
#include <fstream>
#include<conio.h>
#include <Windows.h>
HANDLE *readyEvents;

int CreateSenders(int sendersCount, char filename[80]){
	readyEvents = new HANDLE[sendersCount];
	char buff[10];
	for(int i = 0; i < sendersCount; i++){
		//creating event
		char eventName[30] = "sender";
		strcat(eventName, itoa(i, buff, 10));
		readyEvents[i] = CreateEvent(NULL, TRUE, FALSE, eventName);
		if (readyEvents[i] == NULL) {
		printf("Creation event failed.");
		return GetLastError();
		}
		//creating process
		char args[50] = "Sender.exe ";
		strcat(args, filename);
		strcat(args, " ");
		strcat(args, eventName);
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		if (!CreateProcess(NULL, args, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		printf("Creating process error.\n");
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		}
	}
	printf("Receiver process created %d senders.\n", sendersCount);
}

char* receiveMessage(char* filename){
	std::fstream in(filename, std::ios::binary | std::ios::in);
	if(!in.is_open()){
		return "Opening file failed.\n";
	}

	if(in.peek() == std::ifstream::traits_type::eof())
		return "Message file is empty.";
	//reading a message
	char res[20];
	in.read(res, 20);
	//rewrite other messages
	in.seekg(0, std::ios::end);
	int n = in.tellg();
	in.seekg(0, std::ios::beg);
	char *temp = new char[n];
	in.read(temp, n);
	in.close();
	in.open(filename, std::ios::binary | std::ios::out);
	in.clear();	
	in.write(temp + 20, n - 20);
	in.close();
	return res;
}

int main() {
	char filename[80];
	printf("Enter the file name and the number of entries.\n>");
	int senderCount;
	std::cin >> filename >> senderCount;
	std::fstream in(filename, std::ios::binary | std::ios::out);
	in.clear();
	in.close();

	HANDLE startALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
	CreateSenders(senderCount, filename);
	WaitForMultipleObjects(senderCount, readyEvents, TRUE, INFINITE);
	std::cout << "All senders are ready. Starting." << std::endl;

	HANDLE fileMutex = CreateMutex(NULL, FALSE, "FILE_ACCESS");
	if(fileMutex == NULL){
		printf("Creation mutex failed.");
		return GetLastError();
	}
	//starting processes
	SetEvent(startALL);
	char tmp[20];
	char message[20];
	while(!std::cin.eof()){
		std::cout << ">";
		std::cin >> tmp;
		WaitForSingleObject(fileMutex, INFINITE);
		printf("%s\n", receiveMessage(filename));
		ReleaseMutex(fileMutex);
	}
	delete[] readyEvents;
}