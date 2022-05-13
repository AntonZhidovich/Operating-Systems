#include <iostream>
#include <conio.h>
#include <windows.h>
#include "employee.h"

const char pipeName[30] = "\\\\.\\pipe\\pipe_name";


int main(int argc, char** argv) {
    HANDLE hReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE hStartEvent = OpenEvent(SYNCHRONIZE, FALSE, "START_ALL");
    if(NULL == hStartEvent || NULL == hReadyEvent){
        std::cerr << "Error in opening start/ready Event."  << std::endl;
        getch();
        return GetLastError();
    }
    SetEvent(hReadyEvent);
    std::cout << "Process is ready." << std::endl;
    WaitForSingleObject(hStartEvent, INFINITE);
    std::cout << "Process is started." << std::endl;

    //connecting to pipe
    HANDLE hPipe;
    while(true) {
        hPipe = CreateFile(
                pipeName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                OPEN_EXISTING, 0, NULL);
        //if valid, stop trying to connect
        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }
        if(!WaitNamedPipe(pipeName, 5000)){
            std::cout << "5 second wait timed out." << std::endl;
            getch();
            return 0;
        }
    }
    std::cout << "Connected to pipe." << std::endl;
    return 0;
}
