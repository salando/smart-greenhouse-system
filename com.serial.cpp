//LINE 10 SET COM BOARD
//CURRENTLY DOESNT STOP (ctrl+c doesnt work sometimes)
// VERIFY AND RUN SKETCH FIRST AND THEN THIS PROGRAM
// MAKE SURE BAUD RATE IS 9600 (LINE 39)


#include <windows.h>
#include <iostream>
int main(){
    const char* port = "\\\\.\\COM4";
    std::cout << port << "\n";
    HANDLE h = CreateFileA(
     port,
     GENERIC_READ | GENERIC_WRITE,
     0,
     NULL,
     OPEN_EXISTING,
     0,
     NULL
    );

    if (h == INVALID_HANDLE_VALUE) {
     std::cout << "Could not open the port.\n";
     std::cout << "Error code: " << GetLastError() << "\n";
     return 1;
    }
    
    std::cout << "Port open.\n";

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(h, &dcb)) {
     std::cout << "GetCommState failed: " << GetLastError() << "\n";
     CloseHandle(h);
     return 1;
    }   

    dcb.BaudRate = 9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;    

    if (!SetCommState(h, &dcb)) {
     std::cout << "SetCommState failed: " << GetLastError() << "\n";
     CloseHandle(h);
     return 1;
    }

    std::cout << "Port configured at 9600 8N1.\n";

    COMMTIMEOUTS t = {0};
    t.ReadIntervalTimeout         = 50;
    t.ReadTotalTimeoutConstant    = 50;
    t.ReadTotalTimeoutMultiplier  = 10;
    t.WriteTotalTimeoutConstant   = 50;
    t.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(h, &t)) {
     std::cout << "SetCommTimeouts failed: " << GetLastError() << "\n";
     CloseHandle(h);
     return 1;
    }

    char  buf[256];
    DWORD n;

    while (true) {
    if (ReadFile(h, buf, sizeof(buf) - 1, &n, NULL) && n > 0) {
         buf[n] = '\0';
         std::cout << buf;
         std::cout.flush();
        }
    }

    CloseHandle(h);
    return 0;


    
}   

