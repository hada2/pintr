#pragma once

#include "pintr.hpp"

#ifdef __GNUC__
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef _WIN64
namespace WINDOWS {
    #include <windows.h>
}
#endif

enum {
    VERBOSE_QUIET   = 0,
    VERBOSE_NORMAL  = 1,
    VERBOSE_INFO    = 2,
    VERBOSE_DEBUG   = 3
};

UINT32 getNowTime();

template<typename T> std::string to_string(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

template<typename T> std::string to_hex(const T& n, UINT64 width = 0) {
    std::ostringstream stm;

    if (width) {
        stm << hex << setfill('0') << setw(width) << n;
    }
    else {
        stm << hex << n;
    }

    return stm.str();
}

template<typename T> std::string to_HEX(const T& n, UINT64 width = 0) {
    std::ostringstream stm;

    if (width) {
        stm << hex << setfill('0') << setw(width) << n;
    }
    else {
        stm << hex << n;
    }

    string str = stm.str();
    transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });

    return str;
}

template<class T> BOOL contain(const std::string& s, const T& v) {
    return s.find(v) != std::string::npos;
}

vector<string> split(const string &, char);
UINT64 from_hex(const string &);
UINT64 from_string(const string &);

//VOID terminateCtrlC();
//WINDOWS::BOOL WINAPI controlHandler(WINDOWS::DWORD signal);

class TableForOutput {
private:
    vector<vector<string>> _table;
    string _caption;

    size_t _getRowLength();
    size_t _getColumnLength();

public:
    TableForOutput(const string& = "");

    VOID clearTable();
    VOID pushRow(const string, const string& = "", const string& = "", const string& = "");
    string genTableString();
};

class Console {
private:
    ofstream* _consoleFile;

public:
    Console();
    ~Console();

    VOID open(string);
    VOID out(const char *, UINT32 = VERBOSE_NORMAL);
    VOID out(const string &, UINT32 = VERBOSE_NORMAL);
    VOID outInfo(const char *, UINT32 = VERBOSE_NORMAL);
    VOID outInfo(const string &, UINT32 = VERBOSE_NORMAL);
    VOID outErr(const char*, UINT32 = VERBOSE_NORMAL);
    VOID outErr(const string &, UINT32 = VERBOSE_NORMAL);
    VOID close();
};

