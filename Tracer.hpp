#pragma once

#include "pintr.hpp"
#include "ThreadInfoManager.hpp"
#include "Magic.hpp"
#include "DisasCache.hpp"
#include "RtnCache.hpp"


enum {
    FLAG_REG_RAX   = 1 << 0,
    FLAG_REG_RBX   = 1 << 1,
    FLAG_REG_RCX   = 1 << 2,
    FLAG_REG_RDX   = 1 << 3,
    FLAG_REG_RSI   = 1 << 4,
    FLAG_REG_RDI   = 1 << 5,
    FLAG_REG_R8    = 1 << 6,
    FLAG_REG_R9    = 1 << 7,
    FLAG_REG_R10   = 1 << 8,
    FLAG_REG_R11   = 1 << 9,
    FLAG_REG_R12   = 1 << 10,
    FLAG_REG_R13   = 1 << 11,
    FLAG_REG_R14   = 1 << 12,
    FLAG_REG_R15   = 1 << 13,
    FLAG_REG_RBP   = 1 << 14,
    FLAG_REG_RSP   = 1 << 15,
    FLAG_REG_XMM0  = 1 << 16,
    FLAG_REG_XMM1  = 1 << 17,
    FLAG_REG_XMM2  = 1 << 18,
    FLAG_REG_XMM3  = 1 << 19,
    FLAG_REG_XMM4  = 1 << 20,
    FLAG_REG_XMM5  = 1 << 21,
    FLAG_REG_XMM6  = 1 << 22,
    FLAG_REG_XMM7  = 1 << 23,
    FLAG_REG_XMM8  = 1 << 24,
    FLAG_REG_XMM9  = 1 << 25,
    FLAG_REG_XMM10 = 1 << 26,
    FLAG_REG_XMM11 = 1 << 27,
    FLAG_REG_XMM12 = 1 << 28,
    FLAG_REG_XMM13 = 1 << 29,
    FLAG_REG_XMM14 = 1 << 30,
    FLAG_REG_XMM15 = 1 << 31,
    FLAG_REG_YMM0  = 1ULL << 32,
    FLAG_REG_YMM1  = 1ULL << 33,
    FLAG_REG_YMM2  = 1ULL << 34,
    FLAG_REG_YMM3  = 1ULL << 35,
    FLAG_REG_YMM4  = 1ULL << 36,
    FLAG_REG_YMM5  = 1ULL << 37,
    FLAG_REG_YMM6  = 1ULL << 38,
    FLAG_REG_YMM7  = 1ULL << 39,
    FLAG_REG_YMM8  = 1ULL << 40,
    FLAG_REG_YMM9  = 1ULL << 41,
    FLAG_REG_YMM10 = 1ULL << 42,
    FLAG_REG_YMM11 = 1ULL << 43,
    FLAG_REG_YMM12 = 1ULL << 44,
    FLAG_REG_YMM13 = 1ULL << 45,
    FLAG_REG_YMM14 = 1ULL << 46,
    FLAG_REG_YMM15 = 1ULL << 47,
};

enum {
    FLAG_REG_R    = 1 << 0,
    FLAG_REG_E    = 1 << 1,
    FLAG_REG_X    = 1 << 2,
    FLAG_REG_H    = 1 << 3,
    FLAG_REG_L    = 1 << 4,
};

class ThreadInfoManager;
class GlobalMagicTable;
class DisasCache;
class RtnCache;

class Tracer {
private:
    static ThreadInfoManager _tiManager;
    static GlobalMagicTable _magic;
    static DisasCache _disasCache;
    static RtnCache _rtnCache;
    
    static UINT32 _initTime;
    static UINT32 _lastRefreshTime;
    static string _osVersion;

    static BOOL _firstInstruction;
    static BOOL _isFirstInstruction();

    // method
    static UINT32 _getRunningTime();
    static BOOL _isFiltered(ADDRINT);
    static BOOL _checkRefreshInterval(UINT32);
    static VOID _enableTracingIfStart(ADDRINT);
    static VOID _disableTracingIfEnd(ADDRINT);
    static VOID _dispInitMessage();
    static VOID _setupRegValue(UINT64&, UINT64&, const string);
    static VOID _dumpMemory(const ADDRINT, const struct MemAddrSize, CONTEXT*);
    static VOID _dumpReg(const ADDRINT, const string);
    static VOID _RewriteBeforeIns(ADDRINT, CONTEXT*);
    static VOID _RewriteAfterIns(ADDRINT, CONTEXT*);

    static string _getOsVersion();
    static string _getImgName(const IMG);
    static string _getImgName(const ADDRINT);
    static string _getRtnName(const ADDRINT);
    static string _getImageInfo(const IMG);
    static string _getSectionInfo(const IMG);
    static string _getRtnInfo(const IMG);
    static string _genInsBin(const INS);
    static string _genMemColumn(const ADDRINT, const size_t);
    static string _genRegColumn(const UINT64, const UINT64, const CONTEXT*);
    static string _genRegFlagColumn(const UINT64);
    static string _genRegXmmValue(const REG, const CONTEXT*);
    static string _genRegYmmValue(const REG, const CONTEXT*);
    static string _genRegGeneral(const CONTEXT*);
    static string _genRegXmm(const CONTEXT*);
    static string _genRegYmm(const CONTEXT*);

    static VOID _parseRegFlag(UINT64&, UINT64&, ADDRINT);
    static VOID _parseXmm(UINT64*, UINT64*, const REG, const CONTEXT*);
    static VOID _parseYmm(UINT64*, UINT64*, UINT64*, UINT64*, const REG, const CONTEXT*);

    // PIN callback functions
    static VOID _AnalysisIns(ADDRINT, CONTEXT*);
    static VOID _AnalysisInsFirst(ADDRINT, CONTEXT*);
    static VOID _AnalysisInsBlanch(ADDRINT, CONTEXT*);
    static VOID _AnalysisInsAfter(ADDRINT, CONTEXT*);
    static VOID _AnalysisRegisterRead(ADDRINT, UINT64, UINT64, CONTEXT*);
    static VOID _AnalysisRegisterWrite(ADDRINT, UINT64, UINT64, CONTEXT*);
    static VOID _AnalysisMemoryRead(ADDRINT, ADDRINT, size_t, CONTEXT*);
    static VOID _AnalysisMemoryWrite(ADDRINT);
    static VOID _AnalysisMemoryWriteBefore(ADDRINT, size_t);
    static VOID _AnalysisApiCall(ADDRINT, ADDRINT, CONTEXT*);
    static VOID _AnalysisException();

public:
    Tracer();

    // PIN callback functions
    static VOID Finalize(INT32, VOID *);
    static VOID Instrumentation(INS, VOID *);
    static VOID ImgInstrumentation(IMG, VOID *);
    static VOID AfterForkInChild(THREADID, const CONTEXT*, VOID*);
};



