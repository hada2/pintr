#pragma once

#include "pintr.hpp"

class ThreadInfo;

class ThreadInfoManager {
private:
    UINT32 _pid;
    PIN_THREAD_UID _mainThreadId;
    map<PIN_THREAD_UID,ThreadInfo*> _mapThreadInfo;

    BOOL _isSubThread(PIN_THREAD_UID);
    BOOL _existThreadInfo(PIN_THREAD_UID);
    string _getTracefileName(PIN_THREAD_UID);
    ThreadInfo *_newThreadInfo(PIN_THREAD_UID);

public:
    ThreadInfoManager();
    ~ThreadInfoManager();

    VOID clearTi(PIN_THREAD_UID);
    VOID refreshTiTracefile(PIN_THREAD_UID);
    VOID writeTiTracefile(PIN_THREAD_UID, const char *);
    VOID writeTiTracefile(PIN_THREAD_UID, const ostringstream &);
    VOID writeTiTracefile(PIN_THREAD_UID, const string &);
    VOID writeTiLine(PIN_THREAD_UID);
    VOID clearTiLine(PIN_THREAD_UID);
    VOID closeTiTracefile(PIN_THREAD_UID, string);

    BOOL initThreadInfoMap(PIN_THREAD_UID);
    BOOL filterTiUntilReturnAddr(PIN_THREAD_UID, ADDRINT);
    BOOL isTiReturned(PIN_THREAD_UID);
    BOOL isInTiLastImgRange(PIN_THREAD_UID, ADDRINT);
    BOOL isInTiLastRtnRange(PIN_THREAD_UID, ADDRINT);
    VOID incTiInsCount(PIN_THREAD_UID);
    VOID incTiLogCount(PIN_THREAD_UID);
    VOID pushTiPcHistory(PIN_THREAD_UID, ADDRINT);

    VOID setPid(UINT32);
    VOID setMainTid(PIN_THREAD_UID);
    VOID setTiBasicInfo(PIN_THREAD_UID, string, string, UINT32, UINT32);
    VOID setTiDisas(PIN_THREAD_UID, ADDRINT, string);
    VOID setTiInsBin(PIN_THREAD_UID, string);
    VOID setTiImgName(PIN_THREAD_UID, string);
    VOID setTiApiName(PIN_THREAD_UID, string);
    VOID setTiBranch(PIN_THREAD_UID, BOOL);
    VOID setTiAllRegs(PIN_THREAD_UID, string);
    VOID setTiMemRead(PIN_THREAD_UID, string);
    VOID setTiMemWrite(PIN_THREAD_UID, string);
    VOID setTiMemWriteAddr(PIN_THREAD_UID, ADDRINT);
    VOID setTiMemWriteSize(PIN_THREAD_UID, size_t);
    VOID setTiRegRead(PIN_THREAD_UID, string);
    VOID setTiRegWrite(PIN_THREAD_UID, string);
    VOID setTiRegFlagRead(PIN_THREAD_UID, string);
    VOID setTiRegFlagWrite(PIN_THREAD_UID, string);
    VOID setTiTracingEnabled(PIN_THREAD_UID, BOOL);
    VOID setTiRetAddr(PIN_THREAD_UID, ADDRINT, string);
    VOID setTiLastImg(PIN_THREAD_UID, ADDRINT, ADDRINT, string);
    VOID setTiLastRtn(PIN_THREAD_UID, ADDRINT, ADDRINT, string);

    vector<PIN_THREAD_UID> getTidList();
    UINT32 getTiInsCount(PIN_THREAD_UID);
    UINT32 getTiLogCount(PIN_THREAD_UID);
    ADDRINT getTiMemWriteAddr(PIN_THREAD_UID);
    ADDRINT getTiRetAddr(PIN_THREAD_UID);
    BOOL getTiTracingEnabled(PIN_THREAD_UID);
    size_t getTiMemWriteSize(PIN_THREAD_UID);
    string getTiApiName(PIN_THREAD_UID);
    string getTiLastImgName(PIN_THREAD_UID);
    string getTiLastRtnName(PIN_THREAD_UID);
};


