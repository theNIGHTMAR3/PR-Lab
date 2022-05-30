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


using namespace std;
namespace fs = std::filesystem;

struct Folder;
struct Finfo;

string tabs = "";

struct Finfo
{
public:
    bool is_file;
    string name;
    __int64 length;
    fs::file_time_type modifocationDate;
    Folder* parent;
};

struct Folder : public Finfo
{
    list<Finfo*> children;
};

//Client
void CreateClient(int portID, string content)
{
    cout << "---Client---" << endl << endl;

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");

    SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET)
    {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
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
        return;
    }

    cout << "Connected to the server" << endl;

    int bytesSent;
    int bytesRecv = SOCKET_ERROR;
    const char *sendbuf = content.c_str();
    char recvbuf[32]="";
    

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

        printf("Bytes received: %ld\n", bytesRecv);
        printf("Received text: %s\n", recvbuf);
    }


    system("pause");
}


//Server
void CreateServer(int portID)
{

    cout << "---Server---" << endl << endl;
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");

    SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET)
    {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
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
        return;
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
    char recvbuf[32] = "";

    bytesRecv = recv(mainSocket, recvbuf, 32, 0);
    printf("Bytes received: %ld\n", bytesRecv);
    printf("Received text: %s\n", recvbuf);

    bytesSent = send(mainSocket, sendbuf, strlen(sendbuf), 0);
    printf("Bytes sent: %ld\n", bytesSent);

    system("pause");
}



//rekursywna struktura dla ścieżki
void GetFiles(string path)
{
    string folderContent = "";

    folderContent += "\n" + path + "\n";


    cout << "Process ID: " << GetCurrentProcessId() << endl;
    cout << "Current Directory: " << path << endl;

    for (auto& entry : fs::directory_iterator(path))
    {
        string dir = entry.path().string();

        //folder
        if (entry.is_directory())
        {
            fs::path p(dir);

            STARTUPINFO si;
            PROCESS_INFORMATION pi;

            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));

            string command = "C://Temp//Lab6//Debug//Lab6.exe " + dir;


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
                return ;
            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            cout << "\t" << entry.path().string() << endl;

        }
        //plik
        else
        {
            folderContent += "\t" + entry.path().string() + "\n";
            cout << "\t" << entry.path().string() << endl;

        }

    }


    //fs::directory_entry* main = new fs::directory_entry(fs::path(path));

}

//wypisa� drzewo dla folderu
void WriteFiles(Folder* folder)
{
    cout << folder->name << endl;

    for (auto file : folder->children)
    {

        Finfo* temp = file;
        while (temp->parent != NULL)
        {
            cout << "\t";
            temp = temp->parent;
        }
        //folder
        if (file->is_file == false)
        {

            WriteFiles((Folder*)file);
        }
        //plik
        else
        {
            cout << file->name << endl;
        }

    }
}



int main(int argc, char* argv[])
{
    Folder* folder;
    int startID;

    cout << (string)argv[0] << endl;
    cout << (string)argv[1] << endl;

    if (argc != 2)
    {
        cout << (int)argv[2] << endl;
    }
    

    //beginning, server
    if (argc == 2)
        startID = 1000;
    //client
    else
    {
        string temp = (string)argv[2];
        startID = stoi(temp);
    }
        


    //GetFiles(string(argv[1]));
    cout << endl << endl << endl;
    //system("pause");

    

    //Server
    if (argc == 2)
    {
        cout << startID << endl;
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        string stringPort = to_string(startID);

        string command = "C://Temp//Lab6//Debug//Lab6.exe ";
        command += "dir ";
        command += stringPort;
        command += " client";


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
            return 0;
        }

        CreateServer(startID);
        //CreateServer(1000);
        cout << startID << endl;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

    }
    //client
    else
    {   
        cout << startID << endl;

        CreateClient(startID,"test ");

        cout << startID << endl;


        //CreateClient(1000,"test ");
    }

    system("pause");
    return 0;
}