#pragma warning(disable : 4996)

#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include <codecvt>
#include <ctype.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <atlstr.h>
#include <winuser.h>
#include <stdlib.h>


using namespace std;
namespace fs = std::filesystem;


//Client
void CreateClient(int portID, string content)
{
   // Sleep(2000);
    cout <<endl<< "--- Client : "<<portID<<" --- " << endl << endl;

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");

    SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET)
    {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return ;
    }

    sockaddr_in service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1"); //adres
    service.sin_port = htons(portID); //port

    if (connect(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("Failed to connect.\n");
        WSACleanup();
        return ;
    }

    cout << "Connected to the server" << endl;

    int bytesSent;
    int bytesRecv = SOCKET_ERROR;
    const char* sendbuf = content.c_str();
    char recvbuf[32] = "";


    //send content
    bytesSent = send(mainSocket, sendbuf, strlen(sendbuf), 0);
    printf("Bytes sent: %ld\n", bytesSent);

    while (bytesRecv == SOCKET_ERROR)
    {
        bytesRecv = recv(mainSocket, recvbuf, 32, 0);

        if (bytesRecv == 0 || bytesRecv == WSAECONNRESET)
        {
            printf("Connection closed.\n");
            break;
        }

        if (bytesRecv < 0)
            return;
    }

    closesocket(mainSocket);
    WSACleanup();
}


//Server
string CreateServer(int portID)
{

    cout <<endl<< "--- Server : " << portID << " ---" << endl << endl;
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");

    SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET)
    {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return "";
    }

    sockaddr_in service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1"); //adres
    service.sin_port = htons(portID); //port


    if (bind(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("bind() failed.\n");
        closesocket(mainSocket);
        return "";
    }

    if (listen(mainSocket, 1) == SOCKET_ERROR)
        printf("Error listening on socket.\n");

    SOCKET acceptSocket = SOCKET_ERROR;
    printf("Waiting for a client to connect...\n");

    while (acceptSocket == SOCKET_ERROR)
    {
        acceptSocket = accept(mainSocket, NULL, NULL);
    }

    printf("Client connected.\n");
    mainSocket = acceptSocket;


    int bytesSent;
    int bytesRecv = SOCKET_ERROR;
    char sendbuf[32] = "Server says hello!";
    char recvbuf[4096] = "";

    //receive content
    bytesRecv = recv(mainSocket, recvbuf, 4096, 0);
    printf("Bytes received: %ld\n", bytesRecv);
    printf("Received text: %s\n", recvbuf);

    bytesSent = send(mainSocket, sendbuf, strlen(sendbuf), 0);

    closesocket(mainSocket);
    WSACleanup();

    return recvbuf;
}



//rekursywna struktura dla ścieżki
void GetFiles(string path, int port)
{
    string folderContent = "";

    folderContent += "\n" + path + "\n";


    cout << "Process ID: " << GetCurrentProcessId() << endl;
    cout << "Current Directory: " << path << endl;


    //just to print folder in console and add its content
    for (auto& entry : fs::directory_iterator(path))
    {
        string dir = entry.path().string();
        cout << "\t" << entry.path().string() << endl;
        folderContent += "\t" + entry.path().string() + "\n";
    }


    for (auto& entry : fs::directory_iterator(path))
    {
        string dir = entry.path().string();

        //folder
        //for each folder create new server and client with port+1
        if (entry.is_directory())
        {
            fs::path p(dir);

            STARTUPINFO si;
            PROCESS_INFORMATION pi;

            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));


            //create client with port+1
            string stringPort = to_string(port+1);

            string command = "C://Temp//Lab6//Debug//Lab6.exe " + dir; //exe + dir
            command += " ";
            command += stringPort; //port
            command += " client"; //client

            TCHAR commandTChar[200];
            _tcscpy_s(commandTChar, CA2T(command.c_str()));

            if (!CreateProcess(NULL,   // No module name (use command line)
                commandTChar,        // Command line
                NULL,           // Process handle not inheritable
                NULL,           // Thread handle not inheritable
                FALSE,          // Set handle inheritance to FALSE
                CREATE_NEW_CONSOLE,              // No creation flags
                NULL,           // Use parent's environment block
                NULL,           // Use parent's starting directory 
                &si,            // Pointer to STARTUPINFO structure
                &pi)           // Pointer to PROCESS_INFORMATION structure
                )
            {
                printf("CreateProcess failed (%d).\n", GetLastError());
                return;
            }
            //create server with port+1
            folderContent+=CreateServer(port+1);

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        //plik, nic nie rób
    }

    //if main folder is finished
    if (port == 1000)
    {
        cout << "FINAL PRINT:" << endl;
        cout << folderContent << endl;
    }
    //if in other folder, create client to send content
    else
        CreateClient(port, folderContent);
}


//MAIN
int main(int argc, char* argv[])
{
    //argv[0] - exe
    //argv[1] - dir
    //argv[2] - port
    //argv[3] - client temp

    int startID;

    //beginning, server
    if (argc == 2)
        startID = 1000;
    //client
    else
    {
        string temp = (string)argv[2];
        startID = stoi(temp);
    }

    //run GetFiles with dir and portID
    GetFiles(string(argv[1]), startID);

    cout << endl << endl << endl;
    system("pause");

    return 0;
}