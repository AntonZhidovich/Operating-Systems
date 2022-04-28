#include <iostream>
#include<Windows.h>
#include <fstream>
int main(int argc, char** argv) {

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


	return 0;
}