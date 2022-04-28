#include <iostream>
#include<Windows.h>
#include <fstream>

void sendMessage(std::ofstream &out, char message[20], char* filename){
	out.open(filename, std::ios::binary | std::ios::app);
	out.write(message, 20);
	out.close();
}

int main(int argc, char** argv) {
	char* filename = argv[1];
	char* eventname =argv[2];
	std::ofstream out(filename, std::ios::binary);
	if (!out.is_open()) {
		std::cout << "File is not open." << std::endl;
		system("pause");
		return -1;
	}
	out.close();

	HANDLE ReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, eventname);
	HANDLE StartAll = OpenEvent(SYNCHRONIZE, FALSE, "START_ALL");
	if (ReadyEvent == NULL || StartAll == NULL) {
		printf("Open event failed.\n");
		system("pause");
		return GetLastError();
	}
	SetEvent(ReadyEvent);
	printf("Ready.\n");
	WaitForSingleObject(StartAll, INFINITE);
	printf("Started.\n");

	HANDLE fileMutex = OpenMutex(SYNCHRONIZE, FALSE, "FILE_ACCESS");
	if(fileMutex == NULL){
		printf("Opening mutex failed.");
		system("pause");
		return GetLastError();
	}

	printf("Enter message or enter CTRL+Z to exit.\n");
	char message[20];
	while(true){
		std::cout << ">";
		std::cin.getline(message, 20, '\n');
		if(std::cin.eof())
			break;
		WaitForSingleObject(fileMutex, INFINITE);
		sendMessage(out, message, filename);
		ReleaseMutex(fileMutex);
	}

	return 0;
}