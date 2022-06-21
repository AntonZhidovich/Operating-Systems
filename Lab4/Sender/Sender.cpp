#include <iostream>
#include<Windows.h>
#include <fstream>

const int MESSAGE_SIZE = 20;

void sendMessage(std::ofstream &out, char message[MESSAGE_SIZE], char* filename){
	out.open(filename, std::ios::binary | std::ios::app);
	out.write(message, MESSAGE_SIZE);
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
	if (NULL == ReadyEvent || NULL == StartAll) {
		printf("Open event failed.\n");
		system("pause");
		return GetLastError();
	}
	SetEvent(ReadyEvent);
	printf("Ready.\n");
	printf("Started.\n");
	HANDLE fileMutex = OpenMutex(SYNCHRONIZE, FALSE, "FILE_ACCESS");
	if(NULL == fileMutex){
		printf("Opening mutex failed.");
		system("pause");
		return GetLastError();
	}

	//objects to control the count of written/read messages
	HANDLE senderSemaphore = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, "MESSAGES_COUNT_SEM");
	HANDLE mesReadEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "MESSAGE_READ");
	if (NULL == senderSemaphore || NULL == mesReadEvent)
		return GetLastError(); 
	//request processing
	WaitForSingleObject(StartAll, INFINITE);
	printf("Enter message or enter CTRL+Z to exit.\n");
	char message[MESSAGE_SIZE];
	while(true){
		std::cout << ">";
		std::cin.getline(message, MESSAGE_SIZE, '\n');
		if(std::cin.eof())
			break;
		//sending a message
		WaitForSingleObject(fileMutex, INFINITE);
		sendMessage(out, message, filename);
		ReleaseMutex(fileMutex);
		//if message file is full, waits for reciever event
		if(TRUE != ReleaseSemaphore(senderSemaphore, 1, NULL)){
			std::cout << "Message file is full. Waiting for receiver." << std::endl;
			ResetEvent(mesReadEvent);
			WaitForSingleObject(mesReadEvent, INFINITE);
			ReleaseSemaphore(senderSemaphore, 1, NULL);
		}
		std::cout << "Message has been sent." << std::endl;
	}
	return 0;
}