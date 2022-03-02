#pragma once

#include "pintr.hpp"
#include "TraceLine.hpp"

class ThreadInfo {
private:
    ofstream* _traceFile;
    deque<ADDRINT> _pcHistory;
    UINT32 _insCount;
    UINT32 _logCount;

    struct {
        ADDRINT low, high;
        string name;
    } _lastImg, _lastRtn;

public:
    ThreadInfo(string s);
    ~ThreadInfo();

    // member
    TraceLine line;
    BOOL isTracingEnabled;
    BOOL isReturned;
    ADDRINT retAddr;
    string runningApiName;

    ADDRINT memWriteAddress;
    size_t memWriteSize;

    // method
    VOID openTracefile(string);
    VOID openTracefileAppend(string);
    VOID writeTiTracefile(string);
    VOID flushTracefile();
    VOID closeTracefile();

    VOID pushPcHistory(ADDRINT);
    ADDRINT getPcHistory(UINT);
    VOID incInsCount();
    VOID incLogCount();
    UINT32 getInsCount();
    UINT32 getLogCount();

    VOID setImgAddrRange(ADDRINT, ADDRINT, string);
    VOID setRtnAddrRange(ADDRINT, ADDRINT, string);
    string getImgName();
    string getRtnName();
    BOOL matchImgRange(ADDRINT);
    BOOL matchRtnRange(ADDRINT);

    string genLine();
    VOID clear();
    VOID clearLine();
};



