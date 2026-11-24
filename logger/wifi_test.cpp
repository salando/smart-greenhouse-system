// wifi_test.cpp
// Connects to the board, reads data lines, writes them to a CSV.

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

struct Reading {
    long   ms;
    double tempC;
    int    light;
    double setTemp;
    int    fan;
    int    vent;
    int    led;
    int    alarm;
};


// Splits one complete line into a Reading.
// Accepts the 9-field format, or the older 4-field one with the
// extra values left at zero. Returns false if the line isn't ours.
bool parseLine(const char* line, Reading& r) {
    if (line[0] != 'D' || line[1] != ',') return false;

    // start from a clean slate, so a short line leaves zeros rather than junk
    r.ms = 0; r.tempC = 0; r.light = 0; r.setTemp = 0;
    r.fan = 0; r.vent = 0; r.led = 0; r.alarm = 0;

    char field[32];
    int  fieldLen = 0;
    int  fieldNum = 0;

    for (int i = 0; ; i++) {
        char c = line[i];

        if (c == ',' || c == '\0') {
            field[fieldLen] = '\0';

            if      (fieldNum == 1) r.ms      = atol(field);
            else if (fieldNum == 2) r.tempC   = atof(field);
            else if (fieldNum == 3) r.light   = atoi(field);
            else if (fieldNum == 4) r.setTemp = atof(field);
            else if (fieldNum == 5) r.fan     = atoi(field);
            else if (fieldNum == 6) r.vent    = atoi(field);
            else if (fieldNum == 7) r.led     = atoi(field);
            else if (fieldNum == 8) r.alarm   = atoi(field);

            fieldNum++;
            fieldLen = 0;

            if (c == '\0') break;
        } else {
            if (fieldLen < 31) field[fieldLen++] = c;
        }
    }

    return fieldNum == 9 || fieldNum == 4;
}


SOCKET connectToBoard(const char* ip, int port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return INVALID_SOCKET;

    sockaddr_in addr = {0};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port        = htons(port);

    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(s);
        return INVALID_SOCKET;
    }

    return s;
}


int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cout << "WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET s = connectToBoard("172.20.10.9", 8080);

    if (s == INVALID_SOCKET) {
        std::cout << "Could not connect. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    std::cout << "Connected. Logging to greenhouse_log.csv" << std::endl;

    std::ofstream csv("greenhouse_log.csv");
    csv << "timestamp,ms,tempC,light,setTemp,fan,vent,led,alarm\n";
    csv.flush();

    char buf[256];       // raw bytes from the network
    char lineBuf[256];   // one line being assembled
    int  lineLen = 0;

    while (true) {
        int n = recv(s, buf, sizeof(buf), 0);
        if (n <= 0) break;

        for (int i = 0; i < n; i++) {
            char c = buf[i];

            if (c == '\n') {
                lineBuf[lineLen] = '\0';

                Reading r;
                if (parseLine(lineBuf, r)) {
                    time_t now = time(0);
                    char   stamp[32];
                    strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

                    csv << stamp      << ","
                        << r.ms       << ","
                        << r.tempC    << ","
                        << r.light    << ","
                        << r.setTemp  << ","
                        << r.fan      << ","
                        << r.vent     << ","
                        << r.led      << ","
                        << r.alarm    << "\n";
                    csv.flush();

                    std::cout << stamp
                              << "  " << r.tempC << "/" << r.setTemp << "C"
                              << "  fan "   << r.fan
                              << "  vent "  << r.vent
                              << "  led "   << r.led
                              << "  alarm " << r.alarm
                              << std::endl;
                }

                lineLen = 0;
            }
            else if (c != '\r') {
                if (lineLen < 255) lineBuf[lineLen++] = c;
            }
        }
    }

    std::cout << "Connection closed." << std::endl;

    closesocket(s);
    WSACleanup();
    return 0;
}
