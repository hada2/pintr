#pragma once

#include "pintr.hpp"

class TraceLine {
private:
    // member
    ADDRINT _pc;
    UINT32 _logCount;
    UINT32 _time;
    BOOL _isBranch;

    string _colPrefix;
    string _colSuffix;
    string _colImgName;
    string _colApiName;
    string _colInsBin;
    string _colDisas;
    string _colAllRegs;
    string _colMemRead;
    string _colMemWrite;
    string _colRegRead;
    string _colRegWrite;
    string _colRegFlagsRead;
    string _colRegFlagsWrite;

public:
    TraceLine();
    ~TraceLine();

    // method
    VOID setBasicInfo(string, string, UINT32, UINT32);
    VOID setPc(ADDRINT);
    VOID setBranch(BOOL);
    VOID setInsBin(string);
    VOID setDisas(string);
    VOID setImgName(string);
    VOID setApiName(string);
    VOID setAllRegs(string);
    VOID setMemRead(string);
    VOID setMemWrite(string);
    VOID setRegRead(string);
    VOID setRegWrite(string);
    VOID setRegFlagRead(string);
    VOID setRegFlagWrite(string);

    string getMemRead();
    string getMemWrite();
    string getRegRead();
    string getRegWrite();

    BOOL emptyMemRead();
    BOOL emptyMemWrite();
    BOOL emptyRegRead();
    BOOL emptyRegWrite();

    string genLine();
    VOID clear();
};


