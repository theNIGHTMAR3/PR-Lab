#pragma warning(disable : 4996)

#include <string>
#include <list>
#include <iostream>
#include <filesystem>
#include <codecvt>
#include <windows.h>
#include <atlstr.h>



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

//rekursywna struktura dla œcie¿ki
Folder* GetFiles(string path)
{

    Folder* folder = new Folder();

    for ( auto& entry : fs::directory_iterator(path))
    {

        string dir = entry.path().string();
        
        //folder
        if (entry.is_directory())
        { 
            fs::path p(dir);

            Finfo* temp = GetFiles(dir);
            temp->is_file = false;
            temp->length = entry.file_size();
            temp->modifocationDate = entry.last_write_time();
            temp->name = entry.path().string();
            temp->parent = folder;

            folder->children.push_back(temp);

        }
        //plik
        else
        {
            Finfo* temp =new Finfo();
            temp->is_file = true;
            temp->length = entry.file_size();
            temp->modifocationDate = entry.last_write_time();
            temp->name = entry.path().string();
            temp->parent = folder;

            folder->children.push_back(temp);
        }

    }

    fs::directory_entry* main = new fs::directory_entry(fs::path(path));

    
    folder->is_file = false;
    folder->length = main->file_size();
    folder->modifocationDate = main->last_write_time();
    folder->name = main->path().string();
    folder->parent = NULL;


    return folder;
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



int main(int argc, TCHAR* argv[])
{
    string path = "./";

    Folder* folder;

    //folder=GetFiles(path);

    //WriteFiles(folder);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));


    string command = "C://Users//Micha³ Kuprianowicz//Desktop//Informatyka//PR//Lab2//Debug//Lab2.exe 1";
    TCHAR commandTChar[100];
    _tcscpy_s(commandTChar, CA2T(command.c_str()));

    int i = 0;

    if (!CreateProcess(NULL,   // No module name (use command line)
        commandTChar,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 0;
    }

    i++;

    cout << getpid()<<"  "<<i << endl;



     //Wait until child process exits.
    WaitForSingleObject(pi.hProcess, 10000);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}