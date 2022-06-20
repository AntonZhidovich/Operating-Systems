#include <iostream>
#include <conio.h>
#include <fstream>
#include <string>
#include <time.h>
#include <algorithm>
#include <process.h>
#include <windows.h>
#include "employee.h"

int empCount;
employee* emps;
HANDLE* hReadyEvents;
CRITICAL_SECTION empsCS;
bool *empIsModifying;
const char pipeName[30] = "\\\\.\\pipe\\pipe_name";
const int MESSAGE_SIZE = 10;

void sortEmps(){
    qsort(emps, empCount, sizeof(employee), empCmp);
}

void writeData(const std::string& fileName){
    std::fstream fin(fileName.c_str(), std::ios::binary | std::ios::out);
    fin.write(reinterpret_cast<char*>(emps), sizeof(employee) * empCount);
    fin.close();
}

void readDataSTD(){
    emps = new employee[empCount];
    std::cout << "Enter ID, name and working hours of each employee:" << std::endl;
    for(int i = 1; i <= empCount; ++i){
        std::cout << "#" << i << ">";
        std::cin >> emps[i-1].num >> emps[i-1].name >> emps[i-1].hours;
    }
}

employee* findEmp(int id){
    employee key;
    key.num = id;
    return reinterpret_cast<employee*>(bsearch(reinterpret_cast<const char*>(&key), reinterpret_cast<const char*>(emps),
                              empCount, sizeof(employee), empCmp));
}

void startPocesses(int count){
    char buff[10];
    for(int i = 0; i < count; i++) {
        char cmdargs[80] = "..\\..\\Client\\cmake-build-debug\\Client.exe ";
        char eventName[50] = "READY_EVENT_";
        itoa(i + 1, buff, MESSAGE_SIZE);
        strcat(eventName, buff);
        strcat(cmdargs, eventName);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        hReadyEvents[i] = CreateEvent(NULL, TRUE, FALSE, eventName);
        if (!CreateProcess(NULL, cmdargs, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                           NULL, NULL, &si, &pi)) {
            printf("Creation process error.\n");
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

DWORD WINAPI messaging(LPVOID p){
    HANDLE hPipe = (HANDLE)p;
    //getting emp with id -1 means for client that error occurred
    employee* errorEmp = new employee;
    errorEmp->num = -1;
    while(true){
        DWORD readBytes;
        char message[MESSAGE_SIZE];
        //receiving a message
        bool isRead = ReadFile(hPipe, message, MESSAGE_SIZE, &readBytes, NULL);
        if(!isRead){
            if(ERROR_BROKEN_PIPE == GetLastError()){
                std::cout << "Client disconnected." << std::endl;
                break;
            }
            else {
                std::cerr << "Error in reading a message." << std::endl;
                break;
            }
        }
        //sending answer
        if(strlen(message) > 0) {
            char command = message[0];
            message[0] = ' ';
            int id = atoi(message);
            DWORD bytesWritten;
            EnterCriticalSection(&empsCS);
            employee* empToSend = findEmp(id);
            LeaveCriticalSection(&empsCS);
            if(NULL == empToSend){
                empToSend = errorEmp;
            }
            else{
                int ind = empToSend -  emps;
                if(empIsModifying[ind])
                    empToSend = errorEmp;
                else{
                    switch (command) {
                        case 'w':
                            printf("Requested to modify ID %d.", id);
                            empIsModifying[ind] = true;
                            break;
                        case 'r':
                            printf("Requested to read ID %d.", id);
                            break;
                        default:
                            std::cout << "Unknown request. ";
                            empToSend = errorEmp;
                    }
                }
            }
            bool isSent = WriteFile(hPipe, empToSend, sizeof(employee), &bytesWritten, NULL);
            if(isSent)
                std::cout << "Answer is sent." << std::endl;
            else
                std::cout << "Error in sending answer." << std::endl;
            //receiving a changed record
            if('w' == command && empToSend != errorEmp){
                isRead = ReadFile(hPipe, empToSend, sizeof(employee), &readBytes, NULL);
                if(isRead){
                    std::cout << "Employee record changed." << std::endl;
                    empIsModifying[empToSend - emps] = false;
                    EnterCriticalSection(&empsCS);
                    sortEmps();
                    LeaveCriticalSection(&empsCS);
                }
                else{
                    std::cerr << "Error in reading a message." << std::endl;
                    break;
                }
            }
        }
    }
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete errorEmp;
    return 0;
}

void openPipes(int clientCount){
    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientCount];
    for(int i = 0; i < clientCount; i++){
        hPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX,
                                       PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                       PIPE_UNLIMITED_INSTANCES,0, 0,INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipe){
            std::cerr << "Create named pipe failed." << std::endl;
            getch();
            return;
        }
        if(!ConnectNamedPipe(hPipe, NULL)){
            std::cout << "No connected clients." << std::endl;
            break;
        }
        hThreads[i] = CreateThread(NULL, 0, messaging, static_cast<LPVOID>(hPipe),0,NULL);
    }
    std::cout << "Clients connected to pipe." << std::endl;
    WaitForMultipleObjects(clientCount, hThreads, TRUE, INFINITE);
    std::cout << "All clients are disconnected." << std::endl;
    delete[] hThreads;
}

int main() {
    //data input
    char filename[50];
    std::cout << "Enter the file name and the count of employees. \n>";
    std::cin >> filename >> empCount;
    readDataSTD();
    writeData(filename);
    sortEmps();

    //creating processes
    InitializeCriticalSection(&empsCS);
    srand(time(0));
    int clientCount = 2 + rand() % 3; //from 2 to 4
    HANDLE hstartALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    empIsModifying = new bool[empCount];
    for(int i = 0; i < empCount; ++i)
        empIsModifying[i] = false;
    hReadyEvents = new HANDLE[clientCount];
    startPocesses(clientCount);
    WaitForMultipleObjects(clientCount, hReadyEvents, TRUE, INFINITE);
    std::cout << "All processes are ready.Starting." << std::endl;
    SetEvent(hstartALL);

    //creating pipes
    openPipes(clientCount);
    for(int i = 0; i < empCount; i++)
        emps[i].print(std::cout);
    std::cout << "Press any key to exit" << std::endl;
    getch();
    DeleteCriticalSection(&empsCS);
    delete[] empIsModifying;
    delete[] hReadyEvents;
    delete[] emps;
    return 0;
}