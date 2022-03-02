#include "ThreadInfo.hpp"
#include "TraceLine.hpp"
#include "Options.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;

ThreadInfo::ThreadInfo(string s) : _pcHistory(PC_HISTORY_SIZE, 0)
{
    openTracefile(s);
    writeTiTracefile("# Start of trace\n");
    clear();
}

ThreadInfo::~ThreadInfo()
{
    clear();
    closeTracefile();
}

string ThreadInfo::genLine()
{
    return line.genLine();
}

VOID ThreadInfo::clear()
{
    _insCount = 0;
    _logCount = 0;

    _lastImg.low = 0;
    _lastImg.high = 0;
    _lastImg.name.clear();
    _lastRtn.low = 0;
    _lastRtn.high = 0;
    _lastRtn.name.clear();

    isTracingEnabled = ((gOpt.startAddress || gOpt.isStartingFromEntryPoint || gOpt.isStartingFromMainImage) ? false : true);

    retAddr = 0;
    isReturned = false;
    runningApiName.clear();

    memWriteAddress = 0;
    memWriteSize = 0;

    clearLine();
}

VOID ThreadInfo::clearLine()
{
    line.clear();
}

VOID ThreadInfo::openTracefile(string str)
{
    _traceFile = new ofstream(str.c_str());
}

VOID ThreadInfo::openTracefileAppend(string str)
{
    _traceFile = new ofstream(str.c_str(), ios::app);
}

VOID ThreadInfo::writeTiTracefile(string str)
{
    *_traceFile << str;
}

VOID ThreadInfo::pushPcHistory(ADDRINT pc)
{
    _pcHistory.push_front(pc);
    _pcHistory.pop_back();
}

ADDRINT ThreadInfo::getPcHistory(UINT number)
{
    return _pcHistory[number];
}

VOID ThreadInfo::incInsCount()
{
    _insCount++;
}

VOID ThreadInfo::incLogCount()
{
    _logCount++;
}

UINT32 ThreadInfo::getInsCount()
{
    return _insCount;
}

UINT32 ThreadInfo::getLogCount()
{
    return _logCount;
}

VOID ThreadInfo::flushTracefile()
{
    *_traceFile << flush;
}

VOID ThreadInfo::closeTracefile()
{
    _traceFile->close();
}

VOID ThreadInfo::setImgAddrRange(ADDRINT low, ADDRINT high, string name)
{
    _lastImg.low = low;
    _lastImg.high = high;
    _lastImg.name = name;
}

string ThreadInfo::getImgName()
{
    return _lastImg.name;
}

BOOL ThreadInfo::matchImgRange(ADDRINT pc)
{
    return (_lastImg.low <= pc) && (pc <= _lastImg.high);
}

VOID ThreadInfo::setRtnAddrRange(ADDRINT low, ADDRINT high, string name)
{
    _lastRtn.low = low;
    _lastRtn.high = high;
    _lastRtn.name = name;
}

string ThreadInfo::getRtnName()
{
    return _lastRtn.name;
}

BOOL ThreadInfo::matchRtnRange(ADDRINT pc)
{
    return (_lastRtn.low <= pc) && (pc <= _lastRtn.high);
}


