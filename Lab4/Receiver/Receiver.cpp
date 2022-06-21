#include <iostream>
#include <fstream>
#include<conio.h>
#include <Windows.h>

HANDLE *readyEvents;
const int FILENAME_SIZE = 80;
const int MESSAGE_SIZE = 20;
const int ARGS_CHAR_SIZE = 50;
int CreateSenders(int sendersCount, char filename[FILENAME_SIZE]){
	readyEvents = new HANDLE[sendersCount];
	char buff[10];
	for(int i = 0; i < sendersCount; i++){
		//creating event
		char eventName[30] = "sender";
		strcat(eventName, itoa(i, buff, 10));
		readyEvents[i] = CreateEvent(NULL, TRUE, FALSE, eventName);
		if (NULL == readyEvents[i]) {
		printf("Creation event failed.");
		return GetLastError();
		}
		//creating process
		char args[ARGS_CHAR_SIZE] = "Sender.exe ";
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
	char res[MESSAGE_SIZE];
	in.read(res, MESSAGE_SIZE);
	//rewrite other messages
	in.seekg(0, std::ios::end);
	int n = in.tellg();
	in.seekg(0, std::ios::beg);
	char *temp = new char[n];
	in.read(temp, n);
	in.close();
	in.open(filename, std::ios::binary | std::ios::out);
	in.clear();	
	in.write(temp + MESSAGE_SIZE, n - MESSAGE_SIZE);
	in.close();
	delete[] temp;
	return res;
}

int main() {
	char filename[FILENAME_SIZE];
	printf("Enter the file name and the number of entries.\n>");
	int senderCount;
	std::cin >> filename >> senderCount;
	std::fstream in(filename, std::ios::binary | std::ios::out);
	in.clear();
	in.close();

	HANDLE startALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
	HANDLE fileMutex = CreateMutex(NULL, FALSE, "FILE_ACCESS");
	if(NULL == fileMutex){
		printf("Mutex creation failed.");
		return GetLastError();
	}
	//objects to handle the count of written/read messages
	HANDLE senderSemaphore = CreateSemaphore(NULL, 0, senderCount, "MESSAGES_COUNT_SEM");
	HANDLE mesReadEvent = CreateEvent(NULL, FALSE, FALSE, "MESSAGE_READ");
	if (NULL == senderSemaphore|| NULL == mesReadEvent)
		return GetLastError(); 

	//starting processing
	CreateSenders(senderCount, filename);
	WaitForMultipleObjects(senderCount, readyEvents, TRUE, INFINITE);
	std::cout << "All senders are ready. Starting." << std::endl;
	SetEvent(startALL);
	char tmp[MESSAGE_SIZE];
	char message[MESSAGE_SIZE];
	while(true){
		std::cout << ">";
		std::cin >> tmp;
		if(std::cin.eof())
			break;
		//if messages not found, it waits
		std::cout << "Waiting for a message." << std::endl;
		WaitForSingleObject(senderSemaphore, INFINITE);
		WaitForSingleObject(fileMutex, INFINITE);
		printf("%s\n", receiveMessage(filename));
		ReleaseMutex(fileMutex);
		SetEvent(mesReadEvent);
	}
	delete[] readyEvents;
}