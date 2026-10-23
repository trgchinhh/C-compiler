#include <fileapi.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <cstdlib>

using namespace std;
using namespace filesystem;

const string default_exefoldername = "bin";
const string default_exefilename = "main";
const string default_pathexe = default_exefoldername + "\\" + default_exefilename; 

void print_banner(){
    string banner = R"(
   _____       _____                     _ _           
 / ____|     / ____|                    (_) |          
| |   ______| |     ___  _ __ ___  _ __  _| | ___ _ __ 
| |  |______| |    / _ \| '_ ` _ \| '_ \| | |/ _ \ '__|
| |____     | |___| (_) | | | | | | |_) | | |  __/ |   
 \_____|     \_____\___/|_| |_| |_| .__/|_|_|\___|_|   
                                  | |                  
                                  |_|                  
    )";
    system("cls");
    cout << banner << endl;
}

int main(){
    print_banner();
    if(!exists(default_exefoldername)){
        cout << "(!) Doesn't exists folder bin. Create ..." << endl;
        create_directories(default_exefoldername);
    } 

    string compilecommand = "g++ src\\*.cpp -o " + default_pathexe;
#ifdef __WIN32
    compilecommand += ".exe"; 
#endif
    system(compilecommand.c_str());
    system(default_pathexe.c_str());

    return 0;
}