#include <iostream>
#include <conio.h>
#include <fstream>
#include <time.h>
#include <windows.h>
#include "employee.h"

HANDLE* hReadyEvents;
const char pipeName[30] = "\\\\.\\pipe\\pipe_name";

void writeData(char filename[50], employee* emps, int n){
    std::fstream fin(filename, std::ios::binary | std::ios::out);
    fin.write(reinterpret_cast<char*>(emps), sizeof(employee)*n);
    fin.close();
}

void startPocesses(int count){
    char buff[10];
    for(int i = 0; i < count; i++) {
        char cmdargs[80] = "..\\..\\Client\\cmake-build-debug\\Client.exe ";
        char eventName[50] = "READY_EVENT_";
        itoa(i + 1, buff, 10);
        strcat(eventName, buff);
        strcat(cmdargs, eventName);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        hReadyEvents[i] = CreateEvent(NULL, TRUE, FALSE, eventName);
        if (!CreateProcess(NULL, cmdargs, NULL, NULL, FALSE,
                           CREATE_NEW_CONSOLE,
                           NULL, NULL, &si, &pi)) {
            printf("Creation process error.\n");
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

void openPipes(int clientCount){
    HANDLE hPipe;
    for(int i = 0; i < clientCount; i++){
        hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX,
                                       PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_WAIT,
                                       PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipe){
            std::cerr << "Create named pipe failed." << std::endl;
            getch();
            return;
        }
        if(!ConnectNamedPipe(hPipe, NULL)){
            std::cout << "No connected clients." << std::endl;
            break;
        }
    }
    std::cout << "Clients connected to pipe." << std::endl;
    CloseHandle(hPipe);
}


int main() {
    //data input
    char filename[50];
    int emplCount;
    std::cout << "Enter the file name and the count of employees. \n>";
    std::cin >> filename >> emplCount;
    employee* emps = new employee[emplCount];
    std::cout << "Enter ID, name and working hours of each employee:" << std::endl;
    for(int i = 1; i <= emplCount; ++i){
        std::cout << "#" << i << ">";
        std::cin >> emps[i-1].num >> emps[i-1].name >> emps[i-1].hours;
    }
    writeData(filename, emps, emplCount);

    //creating processes
    srand(time(0));
    int clientCount = 2 + rand() % 3; //from 2 to 4
    HANDLE hstartALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    hReadyEvents = new HANDLE[clientCount];
    startPocesses(clientCount);
    WaitForMultipleObjects(clientCount, hReadyEvents, TRUE, INFINITE);
    std::cout << "All processes are ready.Starting." << std::endl;
    SetEvent(hstartALL);

    //creating pipes
    openPipes(clientCount);
    getch();
    delete[] hReadyEvents;
    delete[] emps;
    return 0;
}


