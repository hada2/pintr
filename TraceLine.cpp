#include "TraceLine.hpp"
#include "Options.hpp"
#include "Utils.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;

TraceLine::TraceLine()
{
    clear();
}

TraceLine::~TraceLine()
{
    clear();
}

string TraceLine::genLine()
{
    string str;

    if (_logCount == 0)
        return "";

    if (gOpt.printLogCount) str += APPEND_DELIM(to_string(_logCount), DELIM);
    if (gOpt.printTime)     str += APPEND_DELIM(to_string(_time), DELIM);

    str += APPEND_DELIM(_colImgName, DELIM);
    str += APPEND_DELIM(_colApiName, DELIM);
    str += APPEND_DELIM(to_hex(_pc), DELIM);
    str += APPEND_DELIM(_colInsBin, DELIM);
    str += APPEND_DELIM(((gOpt.printBranch && _isBranch) ? "*" : "") + _colDisas, DELIM);
    str += APPEND_DELIM(_colRegFlagsRead, DELIM);
    str += APPEND_DELIM(_colRegFlagsWrite, DELIM);
    
    str += string(DELIM);
    str += string("{") + JOIN(_colRegWrite, _colMemWrite, ",") + string("}<=");
    str += string("{") + JOIN(_colRegRead, _colMemRead, ",") + string("}");

    str += APPEND_DELIM(_colAllRegs, DELIM);
    str += "\n";

    return str;
}

VOID TraceLine::setBasicInfo(string prefix, string suffix, UINT32 logCount, UINT32 time)
{
    _colPrefix = prefix;
    _colSuffix = suffix;
    _logCount = logCount;
    _time = time;
}

VOID TraceLine::setPc(ADDRINT pc)
{
    _pc = pc;
}

VOID TraceLine::setImgName(string imgName)
{
    _colImgName = imgName;
}

VOID TraceLine::setApiName(string runningApiName)
{
    _colApiName = runningApiName;
}

VOID TraceLine::setBranch(BOOL isBranch)
{
    _isBranch = isBranch;
}

VOID TraceLine::setInsBin(string insBin)
{
    _colInsBin = insBin;
}

VOID TraceLine::setDisas(string disas)
{
    _colDisas = disas;
}

VOID TraceLine::setAllRegs(string allRegs)
{
    _colAllRegs = allRegs;
}

VOID TraceLine::setMemRead(string memRead)
{
    _colMemRead = memRead;
}

VOID TraceLine::setMemWrite(string memWrite)
{
    _colMemWrite = memWrite;
}

VOID TraceLine::setRegRead(string regRead)
{
    _colRegRead = regRead;
}

VOID TraceLine::setRegWrite(string regWrite)
{
    _colRegWrite = regWrite;
}

VOID TraceLine::setRegFlagRead(string regFlagsRead)
{
    _colRegFlagsRead = regFlagsRead;
}

VOID TraceLine::setRegFlagWrite(string regFlagsWrite)
{
    _colRegFlagsWrite = regFlagsWrite;
}

string TraceLine::getMemRead()
{
    return _colMemRead;
}

string TraceLine::getMemWrite()
{
    return _colMemWrite;
}

string TraceLine::getRegRead()
{
    return _colRegRead;
}

string TraceLine::getRegWrite()
{
    return _colRegWrite;
}

BOOL TraceLine::emptyMemRead()
{
    return _colMemRead.empty();
}

BOOL TraceLine::emptyMemWrite()
{
    return _colMemWrite.empty();
}

BOOL TraceLine::emptyRegRead()
{
    return _colRegRead.empty();
}

BOOL TraceLine::emptyRegWrite()
{
    return _colRegWrite.empty();
}

VOID TraceLine::clear()
{
    _logCount = 0;
    _time = 0;
    _pc = 0;

    _isBranch = false;

    _colPrefix.clear();
    _colSuffix.clear();
    _colImgName.clear();
    _colApiName.clear();
    _colInsBin.clear();
    _colDisas.clear();
    _colAllRegs.clear();
    _colMemRead.clear();
    _colMemWrite.clear();
    _colRegRead.clear();
    _colRegWrite.clear();
}

