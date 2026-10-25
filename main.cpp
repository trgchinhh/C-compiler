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

#ifdef _WIN32               // Nếu là Windows 
    #define NOMINMAX
    #include <windows.h>
    #include <conio.h>
#else                       // Nếu là Linux hoặc HDH khác
    #include <sys/ioctl.h>
    #include <unistd.h> 
    #include <termios.h>
#endif

#include <iostream>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;
using namespace filesystem;
using namespace chrono;
using namespace this_thread;

#define RESET   "\033[0m"
#define RED     "\033[91m"
#define GREEN   "\033[92m"
#define YELLOW  "\033[93;1m"
#define BLUE    "\033[96m"

const string thumuc_chuacodemacdinh = "src";
const string thumuc_chuafileexemacdinh = "bin";
const string ten_fileexemacdinh = "main";
string duongdan_fileexe = thumuc_chuafileexemacdinh + "\\"; 
atomic<bool> batdauspinner(false);

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
        Sleep(0);
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
            if (file.path().extension() == ".c" 
             || file.path().extension() == ".cpp") {
                return true;
            }
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

bool CanBienDich() {
    if(!exists(duongdan_fileexe))
        return true;
    auto thoigiandichexe = last_write_time(duongdan_fileexe);
    for(auto &file : recursive_directory_iterator(thumuc_chuacodemacdinh)){
        if(file.path().extension() == ".cpp" 
        || file.path().extension() == ".h"
        || file.path().extension() == ".c"
        || file.path().extension() == ".hpp"){
            if(last_write_time(file) > thoigiandichexe)
                return true;
        }
    }
    return false;
}

bool KiemTraKetQuaBienDich(string lenhbiendich){
    batdauspinner = true;
    int ketqua = system(lenhbiendich.c_str());
    if(ketqua != 0){
        return false;
    }
    return true;
}

void ChayChuongTrinh(string tenfileexe, string duongdanfileexe){
    system("cls");
    system(duongdanfileexe.c_str()); 
}

bool ChuanDoanTrinhDichVaBienDichFile(string tenfileexe){
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
        cout << RED << "(!) Trình biên dịch quá cũ, yêu cầu C++11 trở lên!" << RESET << endl;
        return false;  
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
        if(!KiemTraKetQuaBienDich(lenhbiendich)){
            return false;
        }
    #elif __GNUC__
             cout << "\t │    ├ " << RESET << YELLOW << "(+)" << " Trình biên dịch: " 
                  << RESET << "GCC / MinGW" << endl;
             cout << "\t └─(*)├ " << RESET << YELLOW << "(+)" << " Phiên bản: " 
                  << RESET << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << endl;
             cout << "\t      ├ " << RESET << YELLOW << "(+)" << " Bản chuẩn: " 
                  << RESET << cpp_name << " (" << cpp_val << ")" << endl;
             cout << "\t      └──────────────────────────────────┘" << endl;
        if(!KiemTraKetQuaBienDich(lenhbiendich)){
            return false;
        }
    #elif __clang__
             cout << "\t │    ├ " << RESET << YELLOW << "(+)" 
                  << " Trình biên dịch: " << RESET << "CLANG" << endl;
             cout << "\t └─(*)├ " << RESET << YELLOW << "(+)" << " Phiên bản: " 
                  << RESET << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << endl;
             cout << "\t      ├ " << RESET << YELLOW << "(+)" << " Bản chuẩn: " 
                  << RESET << cpp_name << " (" << cpp_val << ")" << endl;
             cout << "\t      └──────────────────────────────────┘" << endl;
        if(!KiemTraKetQuaBienDich(lenhbiendich)){
            return false;
        }
    #else 
        cout << RED << "\t(!)  Trình biên dịch không được hỗ trợ !!!" << endl;
        return false;
    #endif 
    // cout << "\n\t(?) Đã biên dịch xong, chạy file: " << tenfileexe << " (y/n): ";
    // char c; cin >> c;
    // if(c == 'y' || c == 'Y'){
    //     ChayChuongTrinh(tenfileexe, duongdanfileexe);
    // }
    return true;
}

void XoaManHinh(){
    #ifdef _WIN32 
        system("cls");
    #else 
        system("clear");
    #endif
}

void AnConTroChuot(bool trangthaicontrochuot){
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = trangthaicontrochuot;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    #else
        if (trangthaicontrochuot) cout << "\033[?25h"; 
        else cout << "\033[?25l";
    #endif
}

void HieuUngSpinner(atomic<bool>& dung){
    while(!batdauspinner && !dung){
        sleep_for(milliseconds(10));
    }
    AnConTroChuot(false);
    const char* spinner[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    int i = 0;
    while(!dung){
        cout << "\r" << spinner[i] 
             << RESET << " Vui lòng chờ biên dịch" << flush;
        i = (i + 1) % 10;
        sleep_for(milliseconds(100));
    }
    cout << "\r                                             \r";
    AnConTroChuot(true);
}

int DocPhimNguoiDung() {
    #ifdef _WIN32
        int c = _getch();
        if (c == 0 || c == 224) {
            switch (_getch()) {
                case 72: return 72; 
                case 80: return 80; 
                case 75: return 75; 
                case 77: return 77; 
            }
        }
        if (c == 13) 
            return 13;
        return c;
    #else
        termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        newt.c_cc[VMIN]  = 1;
        newt.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        int c = getchar();
        if (c == 27) {
            int c1 = getchar();
            if (c1 == '[') {
                int c2 = getchar();
                switch (c2) {
                    case 'A': c = 72; break; 
                    case 'B': c = 80; break; 
                    case 'C': c = 77; break; 
                    case 'D': c = 75; break; 
                }
            }
        }
        if (c == 10) 
            c = 13;
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return c;
    #endif
}

int DoRongHienThi(const char* s){
    int chieurong = 0;
    for(int i = 0; s[i] != '\0'; ){
        unsigned char c = s[i];
        if(c < 128){ 
            chieurong += 1; i += 1; 
        } else{ 
            chieurong += 1; 
            i += 2; 
        }
    }
    return chieurong;
}

void Menu(string tenfileexe, string duongdanfileexe){
    const int somuc = 2;
    const char* menu[somuc] = {
        "Chạy file",
        "Thoát"
    };
    int chon = 0;
    while(true){
        XoaManHinh();
        InBanner();
        cout << "Không có thay đổi -> dùng file build cũ" << RESET << endl;
        cout << "┌───────────────┐" << RESET << endl;

        for (int i = 0; i < somuc; i++) {
            if (i == chon) {
                cout << "│" << RED << " ● " << RESET << menu[i];
                int spaces = 12 - DoRongHienThi(menu[i]);
                cout << RESET << string(spaces, ' ') << "│" << endl;
            } else {
                cout << "│   " << menu[i];
                int spaces = 12 - DoRongHienThi(menu[i]);
                cout << RESET << string(spaces, ' ') << "│" << endl;
            }
        }
        cout << "└───────────────┘" << RESET << "\n";
        int phim = DocPhimNguoiDung();
        if (phim == 72) 
            chon = (chon - 1 + somuc) % somuc;
        else if (phim == 80)
            chon = (chon + 1) % somuc;
        else if (phim == 13) {
            if(chon == 0){
                ChayChuongTrinh(tenfileexe, duongdanfileexe);
                AnConTroChuot(true);
                exit(0);
            } 
            else if(chon == 1){
                AnConTroChuot(true);
                exit(0);
            }
        }
    }
}

int main(){
    system("cls");
    AnConTroChuot(false);

    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);

    string tenfileexe = "";

    InBanner();
    if(!exists(thumuc_chuafileexemacdinh)){
        cout << "(!) Chưa tồn tại thư mục bin. Đang tạo ..." << endl;
        create_directories(thumuc_chuafileexemacdinh);
        NhapTenFileExe(tenfileexe);
    } else {
        for(const auto& file : directory_iterator(thumuc_chuafileexemacdinh)){
            if(file.path().extension() == ".exe"){
                tenfileexe = file.path().stem().string();
                break;
            }
        }
        if(tenfileexe.empty()){
            tenfileexe = ten_fileexemacdinh;
        }
    }
    duongdan_fileexe = thumuc_chuafileexemacdinh + "\\" + tenfileexe;
    #ifdef __WIN32
        duongdan_fileexe += ".exe";
    #endif 
    if(CanBienDich()){
        cout << "(!) Tìm thấy file cần biên dịch: ";
        if(!TimFileExe()){
            cout << RED << "Không" << RESET << endl;
        } else {
            cout << GREEN <<"Có" << RESET << endl;
        }
        atomic<bool> stop(false);
        thread loading(HieuUngSpinner, ref(stop));
        int ketqua = ChuanDoanTrinhDichVaBienDichFile(tenfileexe);
        stop = true;
        loading.join();
        if(!ketqua){
            throw runtime_error(RED "Biên dịch thất bại !");
        }
        cout << "Biên dịch thành công !" << endl;
        cout << "\n\t(?) Đã biên dịch xong, chạy file: " << tenfileexe << " (y/n): ";
        char c; cin >> c;
        if(c == 'y' || c == 'Y'){
            ChayChuongTrinh(tenfileexe, duongdan_fileexe);
        }
    } else {
        Menu(tenfileexe, duongdan_fileexe);
    }

    AnConTroChuot(true);
    return 0;
}