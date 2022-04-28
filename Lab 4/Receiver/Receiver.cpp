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

	//starting processes
	SetEvent(startALL);
	system("pause");
	delete[] readyEvents;
}