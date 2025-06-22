#include "ThreadInfoManager.hpp"
#include "ThreadInfo.hpp"
#include "Options.hpp"
#include "Utils.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;
extern Console gConsole;

ThreadInfoManager::ThreadInfoManager()
{
    _pid = 0;
}

ThreadInfoManager::~ThreadInfoManager()
{
    for (PIN_THREAD_UID tid : getTidList()) {
        clearTi(tid);
    }
}

VOID ThreadInfoManager::clearTi(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];

    if (ti) {
        delete ti;
    }

    _mapThreadInfo.erase(tid);
}

BOOL ThreadInfoManager::initThreadInfoMap(PIN_THREAD_UID tid)
{
    if (!_existThreadInfo(tid)) {
        if (_isSubThread(tid)) {
            gConsole.outInfo("New thread is detected (tid=" + std::to_string(tid) + ")\n");
        }

        if (gOpt.modeMainThreadOnly) {
            if (_isSubThread(tid)) {
                _mapThreadInfo[tid] = nullptr;
                return false;
            }
        }

        if (_mapThreadInfo.size() >= THREAD_MAX) {
            _mapThreadInfo[tid] = nullptr;
            gConsole.outErr("Too many threads\n");
            return false;
        }

        _mapThreadInfo[tid] = new ThreadInfo(_getTracefileName(tid));

        assert(_mapThreadInfo[tid]);
    }

    if (!_mapThreadInfo[tid]) {
        return false;
    }

    return true;
}

VOID ThreadInfoManager::setMainTid(PIN_THREAD_UID tid)
{
    _mainThreadId = tid;
}

VOID ThreadInfoManager::setPid(UINT32 pid)
{
    _pid = pid;
}

BOOL ThreadInfoManager::_isSubThread(PIN_THREAD_UID tid)
{
    return tid != _mainThreadId;
}

string ThreadInfoManager::_getTracefileName(PIN_THREAD_UID tid)
{
    string sub;

    if (_pid) {
        sub += "-child" + std::to_string(_pid);
    }

    if (_isSubThread(tid)) {
        sub += "-th" + std::to_string(tid);
    }

    return gOpt.tracefileBaseName + sub + ".log";
}

BOOL ThreadInfoManager::_existThreadInfo(PIN_THREAD_UID tid)
{
    return _mapThreadInfo.count(tid) > 0;
}

BOOL ThreadInfoManager::isInTiLastImgRange(PIN_THREAD_UID tid, UINT64 pc)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->matchImgRange(pc);
}

BOOL ThreadInfoManager::isInTiLastRtnRange(PIN_THREAD_UID tid, ADDRINT pc)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->matchRtnRange(pc);
}

string ThreadInfoManager::getTiLastImgName(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->getImgName();
}

string ThreadInfoManager::getTiLastRtnName(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->getRtnName();
}

ADDRINT ThreadInfoManager::getTiMemWriteAddr(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->memWriteAddress;
}

size_t ThreadInfoManager::getTiMemWriteSize(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->memWriteSize;
}

VOID ThreadInfoManager::setTiLastImg(PIN_THREAD_UID tid, ADDRINT low, ADDRINT high, string name)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->setImgAddrRange(low, high, name);
}

VOID ThreadInfoManager::setTiLastRtn(PIN_THREAD_UID tid, ADDRINT low, ADDRINT high, string name)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->setRtnAddrRange(low, high, name);
}

VOID ThreadInfoManager::incTiInsCount(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->incInsCount();
}

VOID ThreadInfoManager::incTiLogCount(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->incLogCount();
}

VOID ThreadInfoManager::pushTiPcHistory(PIN_THREAD_UID tid, ADDRINT pc)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->pushPcHistory(pc);
}

ADDRINT ThreadInfoManager::getTiLastPcHistory(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->getPcHistory(1);
}

UINT32 ThreadInfoManager::getTiInsCount(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->getInsCount();
}

UINT32 ThreadInfoManager::getTiLogCount(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->getLogCount();
}

VOID ThreadInfoManager::setTiMemWriteAddr(PIN_THREAD_UID tid, ADDRINT memAddr)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->memWriteAddress = memAddr;
}

VOID ThreadInfoManager::setTiMemWriteSize(PIN_THREAD_UID tid, size_t memSize)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->memWriteSize = memSize;
}

VOID ThreadInfoManager::writeTiTracefile(PIN_THREAD_UID tid, const char *str)
{
    PIN_GetLock(&gLockWrite, tid + 1);

    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->writeTiTracefile(str);
    ti->flushTracefile();

    PIN_ReleaseLock(&gLockWrite);
}

VOID ThreadInfoManager::writeTiTracefile(PIN_THREAD_UID tid, const ostringstream &ss)
{
    writeTiTracefile(tid, ss.str().c_str());
}

VOID ThreadInfoManager::writeTiTracefile(PIN_THREAD_UID tid, const string &s)
{
    writeTiTracefile(tid, s.c_str());
}

VOID ThreadInfoManager::refreshTiTracefile(PIN_THREAD_UID tid)
{
    PIN_GetLock(&gLockWrite, PIN_ThreadUid() + 1);

    ThreadInfo* ti = _mapThreadInfo[tid];

    if (ti) {
        ti->flushTracefile();
        ti->closeTracefile();
        ti->openTracefileAppend(_getTracefileName(tid));
    }

    PIN_ReleaseLock(&gLockWrite);
}

VOID ThreadInfoManager::closeTiTracefile(PIN_THREAD_UID tid, string close_msg)
{
    PIN_GetLock(&gLockWrite, PIN_ThreadUid() + 1);

    ThreadInfo* ti = _mapThreadInfo[tid];

    if (ti) {
        ti->writeTiTracefile(close_msg);
        ti->closeTracefile();
    }

    PIN_ReleaseLock(&gLockWrite);
}

vector<PIN_THREAD_UID> ThreadInfoManager::getTidList()
{
    vector<PIN_THREAD_UID> tidList;

    for (auto iter = _mapThreadInfo.begin(); iter != _mapThreadInfo.end(); iter++) {
        PIN_THREAD_UID tid = iter->first;
        tidList.push_back(tid);
    }

    return tidList;
}

VOID ThreadInfoManager::setTiTracingEnabled(PIN_THREAD_UID tid, BOOL isTracingEnabled)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->isTracingEnabled = isTracingEnabled;
}

VOID ThreadInfoManager::setTiRetAddr(PIN_THREAD_UID tid, ADDRINT retAddr, string apiName)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->retAddr = retAddr;
    ti->runningApiName = apiName;
}

ADDRINT ThreadInfoManager::getTiRetAddr(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->retAddr;
}

BOOL ThreadInfoManager::getTiTracingEnabled(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->isTracingEnabled;
}

string ThreadInfoManager::getTiApiName(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->runningApiName;
}

VOID ThreadInfoManager::setTiBasicInfo(PIN_THREAD_UID tid, string prefix, string suffix, UINT32 logCount, UINT32 time)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setBasicInfo(prefix, suffix, logCount, time);
}

VOID ThreadInfoManager::setTiDisas(PIN_THREAD_UID tid, ADDRINT pc, string disas)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setPc(pc);
    ti->line.setDisas(disas);
}

VOID ThreadInfoManager::setTiInsBin(PIN_THREAD_UID tid, string insBin)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setInsBin(insBin);
}

VOID ThreadInfoManager::setTiImgName(PIN_THREAD_UID tid, string imgName)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setImgName(imgName);
}

VOID ThreadInfoManager::setTiApiName(PIN_THREAD_UID tid, string apiName)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setApiName(apiName);
}

VOID ThreadInfoManager::setTiAllRegs(PIN_THREAD_UID tid, string allRegs)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setAllRegs(allRegs);
}

VOID ThreadInfoManager::setTiMemRead(PIN_THREAD_UID tid, string memRead)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    string str = ti->line.getMemRead();

    if (str.empty()) {
        str += memRead;
    }
    else {
        str += APPEND_DELIM(memRead, ",");
    }

    ti->line.setMemRead(str);
}

VOID ThreadInfoManager::setTiMemWrite(PIN_THREAD_UID tid, string memWrite)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    string str = ti->line.getMemWrite();

    if (str.empty()) {
        str += memWrite;
    }
    else {
        str += APPEND_DELIM(memWrite, ",");
    }

    ti->line.setMemWrite(str);
}

VOID ThreadInfoManager::setTiRegRead(PIN_THREAD_UID tid, string regRead)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    string str = ti->line.getRegRead();

    if (str.empty()) {
        str += regRead;
    }
    else {
        str += APPEND_DELIM(regRead, ",");
    }

    ti->line.setRegRead(str);
}

VOID ThreadInfoManager::setTiRegWrite(PIN_THREAD_UID tid, string regWrite)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    string str = ti->line.getRegWrite();

    if (str.empty()) {
        str += regWrite;
    }
    else {
        str += APPEND_DELIM(regWrite, ",");
    }

    ti->line.setRegWrite(str);
}

VOID ThreadInfoManager::setTiRegFlagRead(PIN_THREAD_UID tid, string regFlagsRead)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);

    if (!regFlagsRead.empty()) {
        regFlagsRead = "RF" + regFlagsRead;
    }

    ti->line.setRegFlagRead(regFlagsRead);
}

VOID ThreadInfoManager::setTiRegFlagWrite(PIN_THREAD_UID tid, string regFlagsWrite)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);

    if (!regFlagsWrite.empty()) {
        regFlagsWrite = "WF" + regFlagsWrite;
    }

    ti->line.setRegFlagWrite(regFlagsWrite);
}

VOID ThreadInfoManager::setTiBranch(PIN_THREAD_UID tid, BOOL isBranch)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->line.setBranch(isBranch);
}

VOID ThreadInfoManager::writeTiLine(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    writeTiTracefile(tid, ti->genLine());
}

VOID ThreadInfoManager::clearTiLine(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->clearLine();
}

BOOL ThreadInfoManager::filterTiUntilReturnAddr(PIN_THREAD_UID tid, ADDRINT pc)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    ti->isReturned = false;

    if (ti->retAddr != 0) {
        if (pc == ti->retAddr) {
            ti->retAddr = 0;
            ti->isReturned = true;
        } else {
            // filter until back to return address
            return true;
        }
    }

    return false;
}

BOOL ThreadInfoManager::isTiReturned(PIN_THREAD_UID tid)
{
    ThreadInfo* ti = _mapThreadInfo[tid];
    assert(ti);
    return ti->isReturned;
}


