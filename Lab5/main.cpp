#pragma warning(disable : 4996)

#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include <codecvt>
#include <windows.h>
#include <atlstr.h>
#include <namedpipeapi.h>


//amount of characters
#define N 1000


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

//writing data into pipe
void WriteIntoPipe(string receivedData)
{
    cout << "Creating a pipe" << endl;

    string myPipe = "\\\\.\\pipe\\myPipe";

    // Creating pipe to send data
    wstring tmp = wstring(myPipe.begin(), myPipe.end());

    const wchar_t* tempName = tmp.c_str();

    HANDLE pipe = CreateNamedPipe(
        tempName, // name of the pipe
        PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
        PIPE_TYPE_BYTE, // send data as a byte stream
        50, // only allow 1 instance of this pipe
        0, // no outbound buffer
        0, // no inbound buffer
        0, // use default wait time
        NULL // use default security attributes
    );

    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE)
    {
        cout << "Failed to create pipe.";
        cout << GetLastError() << endl;
        system("pause");
        return;
    }

    cout << "Waiting for a client" << endl;
    // This call blocks until a client process connects to the pipe

    BOOL result;
    result = ConnectNamedPipe(pipe, NULL);

    cout << "Sending data to pipe" << endl;

    // This call blocks until a client process reads all the data
    tmp = wstring(receivedData.begin(), receivedData.end());

    const wchar_t* dataToSend = tmp.c_str();
    DWORD bytesWritten = 0;

    result = WriteFile
    (
        pipe, // handle to outbound pipe
        dataToSend, // data to send
        wcslen(dataToSend) * sizeof(wchar_t), // length of data to send (bytes)
        &bytesWritten, // will store actual amount of data sent
        NULL // not using overlapped IO
    );

    if (result)
    {
        cout << "Sent bytes: " << bytesWritten << endl;
    }
    else
    {
        cout << "Failed to send data." << endl;
        cout << GetLastError() << endl;
        system("pause");
        return;
    }

    cout << "Done, closing the pipe" << endl;


    // Close the pipe (automatically disconnects client too)
    CloseHandle(pipe);
}



//reading data from pipe
string ReadFromPipe()
{

    string myPipe = "\\\\.\\pipe\\myPipe";

    // Open the named pipe
    wstring tmp = wstring(myPipe.begin(), myPipe.end());
    const wchar_t* nameW = tmp.c_str();

    HANDLE pipe;
    do
    {
        pipe = CreateFile(
            L"\\\\.\\pipe\\myPipe",
            GENERIC_READ, // only need read access
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
    } while (pipe == INVALID_HANDLE_VALUE);
    //waiting for process to create a pipe first
    Sleep(10);

    // buffer for N characters
    wchar_t buffer[N + 1];
    DWORD bytesRead = 0;
    BOOL result;

    result = ReadFile(
        pipe,
        buffer, // the data from the pipe will be put here
        N * sizeof(wchar_t), // number of bytes allocated
        &bytesRead, // this will store number of bytes actually read
        NULL // not using overlapped IO
    );

    if (result)
    {
        buffer[bytesRead / sizeof(wchar_t)] = '\0'; // null terminate the string
    }
    else 
    {
        cout << "Failed to read data from the pipe." << endl;
        cout << GetLastError() << endl;
        return "ERROR";
    }
    // Close our pipe handle
    CloseHandle(pipe);


    wstring wString(buffer);
 
    string output(wString.begin(), wString.end());

    return output;
}




//rekursywna struktura dla œcie¿ki
void GetFiles(string path,bool isMain)
{
    string folderContent;
    folderContent += "\n"+path + '\n';


    Folder* folder = new Folder();

    cout << "Process ID: " << GetCurrentProcessId() << endl;
    cout << "Current Directory: " << path << endl;

    for ( auto& entry : fs::directory_iterator(path))
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

            string command = "C://Temp//Lab5//Debug//Lab5.exe " + dir;
            command += " notMain";
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
            
            cout <<"\t"<< entry.path().string() << endl;
        }
        //plik
        else
        {
            folderContent += "\t"+entry.path().string()+"\n";

            cout << "\t" << entry.path().string() << endl;
        }

    }

    if(isMain==false)
        WriteIntoPipe(folderContent);

    return ;
}

//wypisaæ drzewo dla folderu
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
            cout <<file->name << endl;
        }
    }
}


//main
int main(int argc, char* argv[])
{
    //to few arguments
    if (argc < 2)
    {
        cout << "Argument error" << endl;
        return 0;
    }

    bool main = true;

    //not main
    if (argc == 3)
        main = false;

    string path = string(argv[1]);

    GetFiles(argv[1],main);


    //if main process
    if (main)
    {
        //write all folders
        while (true)
            cout << ReadFromPipe();
    }

    system("pause");


    return 0;
}