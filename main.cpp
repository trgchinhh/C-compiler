/*
   _____       _____                      _ _           
  / ____|     / ____|                    (_) |          
 | |   ______| |     ___  _ __ ___  _ __  _| | ___ _ __ 
 | |  |______| |    / _ \| '_ ` _ \| '_ \| | |/ _ \ '__|
 | |____     | |___| (_) | | | | | | |_) | | |  __/ |   
  \_____|     \_____\___/|_| |_| |_| .__/|_|_|\___|_|   
                                   | |                  
                                   |_|                  
                                         - Tool biên dịch tự động nhiều file C++ 
                                         - Biên dịch nhiều file và nhanh chóng 
                                         - Tác giả: Nguyễn Trường Chinh 
                                         - Github: https://github.com/trgchinhh
*/

#include <fileapi.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <windows.h>

using namespace std;
using namespace filesystem;

#define RESET   "\033[0m"
#define RED     "\033[91m"
#define GREEN   "\033[92m"
#define YELLOW  "\033[93;1m"
#define BLUE    "\033[96m"

const string thumuc_chuacodemacdinh = "src";
const string thumuc_chuafileexemacdinh = "bin";
const string ten_fileexemacdinh = "main";
string duongdan_fileexe = thumuc_chuafileexemacdinh + "\\"; 

int LayChieuRongTerminal(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int chieurong;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        chieurong = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return chieurong;
}

void InGiuaTerminal(const string& line, string color) {
    int chieu_rong = LayChieuRongTerminal();  
    int do_dai = line.length();
    int khoang_trang = (chieu_rong - do_dai) / 2;
    if (khoang_trang < 0) khoang_trang = 0;
    cout << string(khoang_trang, ' ') << line;
}

void InBanner(){
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
    stringstream ss(banner);
    string line;
    // cout << endl;
    while (getline(ss, line)) {
        InGiuaTerminal(line, RESET);
        cout << endl;
        Sleep(10);
    }
}

void NhapTenFileExe(string& tenfileexe){
    do {
        cout << "(?) Nhập tên <file>.exe (Không đuôi - không cách): ";
        getline(cin, tenfileexe);
        if(tenfileexe.empty()){
           tenfileexe = ten_fileexemacdinh;
           break;
        }
    } while(tenfileexe.empty());
    duongdan_fileexe += tenfileexe;
}

bool TimFileExe() {
    try {
        for (const auto &file : std::filesystem::directory_iterator(thumuc_chuafileexemacdinh)) {
            if (file.path().extension() == ".cpp") {
                return true;
            }
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

void ChuanDoanTrinhDichVaBienDichFile(string tenfileexe){
    cout << "\t(-)" << " Đang biên dịch chương trình! Vui lòng chờ ...";
    cout << "\n\t │\n";
    cout << "\t │    ┌─────── " << YELLOW << "Thông tin trình dịch" << RESET << " ─────┐" << endl;

    struct CppVersion { 
        long long value; 
        const char* name; 
    };
    CppVersion cppvs[] = {
        {199711L, "C++98"},
        {201103L, "C++11"},
        {201402L, "C++14"},
        {201703L, "C++17"},
        {202002L, "C++20"}
    };
    const char* cpp_name = "Chuẩn C++ không xác định";
    long long cpp_val = __cplusplus;
    for (auto &s : cppvs) {
        if (cpp_val == s.value) {
            cpp_name = s.name;
            break;
        }
    }

    string lenhbiendich = "";
    #ifdef _MSC_VER
        lenhbiendich = "cl ";
    #elif __GNUC__
        lenhbiendich = "g++ ";
    #elif __clang__
        lenhbiendich = "clang++ ";
    #endif

    lenhbiendich += thumuc_chuacodemacdinh + "\\*.cpp ";

    #ifdef _MSC_VER
        lenhbiendich += "/FE ";
    #else
        lenhbiendich += "-o ";
    #endif

    #ifdef __WIN32 
        tenfileexe = tenfileexe + ".exe";
    #endif
    lenhbiendich += thumuc_chuafileexemacdinh + "\\" + tenfileexe;
    string duongdanfileexe = thumuc_chuafileexemacdinh + "\\" + tenfileexe; 

    if (cpp_val <= cppvs[0].value) {
        cerr << RED << "(!) Trình biên dịch quá cũ, yêu cầu C++11 trở lên!" << RESET << endl;
        return;  
    }

    if (cpp_val > cppvs[4].value) cpp_name = "C++ mới hơn C++20";

    #ifdef _MSC_VER
             cout << "\t │    ├ " << RESET << YELLOW << "(+)" 
                  << " Trình biên dịch: " << RESET << "MSVC" << endl;
             cout << "\t └─(*)├ " << RESET << YELLOW << "(+)" 
                  << " Phiên bản: " << RESET << _MSC_VER << endl;
             cout << "\t      ├ " << RESET << YELLOW << "(+)" 
                  << " Bản chuẩn: " << RESET << cpp_name << " (" << cpp_val << ")" << endl;
             cout << "\t      └──────────────────────────────────┘" << endl;
        system(lenhbiendich.c_str());
    #elif __GNUC__
             cout << "\t │    ├ " << RESET << YELLOW << "(+)" << " Trình biên dịch: " 
                  << RESET << "GCC / MinGW" << endl;
             cout << "\t └─(*)├ " << RESET << YELLOW << "(+)" << " Phiên bản: " 
                  << RESET << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
             cout << "\t      ├ " << RESET << YELLOW << "(+)" << " Bản chuẩn: " 
                  << RESET << cpp_name << " (" << cpp_val << ")" << endl;
             cout << "\t      └──────────────────────────────────┘" << endl;
        system(lenhbiendich.c_str());
    #elif __clang__
             cout << "\t │    ├ " << RESET << YELLOW << "(+)" 
                  << " Trình biên dịch: " << RESET << "CLANG" << endl;
             cout << "\t └─(*)├ " << RESET << YELLOW << "(+)" << " Phiên bản: " 
                  << RESET << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << endl;
             cout << "\t      ├ " << RESET << YELLOW << "(+)" << " Bản chuẩn: " 
                  << RESET << cpp_name << " (" << cpp_val << ")" << endl;
             cout << "\t      └──────────────────────────────────┘" << endl;
        system(lenhbiendich.c_str());
    #else 
        cout << RED << "\t(!)  Trình biên dịch không được hỗ trợ !!!" << endl;
        return -1;
    #endif 


    cout << "\n\t(?) Đã biên dịch xong, chạy file: " << tenfileexe << " (y/n): ";
    char c; cin >> c;
    if(c == 'y' || c == 'Y'){
        system("cls");
        system(duongdanfileexe.c_str()); 
    } 
}

int main(){
    system("cls");
    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);

    string tenfileexe = "";

    InBanner();
    if(!exists(thumuc_chuafileexemacdinh)){
        cout << "(!) Chưa tồn tại thư mục bin. Đang tạo ..." << endl;
        create_directories(thumuc_chuafileexemacdinh);
        NhapTenFileExe(tenfileexe);
    } 
    if(!exists(duongdan_fileexe)){
        cout << "(!) Chưa tồn tại file " << duongdan_fileexe << ". Đang tạo ..." << endl;
        create_directories(duongdan_fileexe);
    }

    cout << "(!) Tìm thấy file cần biên dịch: ";
    if(!TimFileExe()){
        cout << RED << "Không" << RESET << endl;
    } else {
        cout << GREEN <<"Có" << RESET << endl;
    }

    ChuanDoanTrinhDichVaBienDichFile(tenfileexe);

    return 0;
}