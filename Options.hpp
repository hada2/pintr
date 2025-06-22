#pragma once

#include "pintr.hpp"
#include "TraceRules.hpp"
#include "Utils.hpp"

class TraceRules;

struct MemAddrSize {
    ADDRINT addr;
    size_t size;
};

class Options {
private:
    map<ADDRINT,struct MemAddrSize> _mapDumpMem;
    map<ADDRINT,int> _mapDumpReg;
    
public:
    Options();

    string tracefileBaseName;

    // trigger
    BOOL isFilteringInsideApi;
    BOOL isFilteringInsideApiOrg;
    BOOL isStartingFromEntryPoint;
    BOOL isStartingFromMainImage;
    ADDRINT startAddress;
    ADDRINT endAddress;
    ADDRINT startAddressRangeLow;
    ADDRINT startAddressRangeHigh;
    UINT32 traceExitTimeLimit;
    UINT32 traceExitInsLimit;

    // rules
    TraceRules traceRules;

    // debug
    UINT32 verboseLevel;

    // mode
    BOOL modeMainThreadOnly;
    BOOL modeFilterExceptJump;
    BOOL modeApiOnly;
    BOOL modeMagicDetect;
    BOOL modeCryptApiDetect;

    // print columns
    BOOL printRegGeneral;
    BOOL printRegFlag;
    BOOL printRegXmm;
    BOOL printRegYmm;
    BOOL printLogCount;
    BOOL printTime;
    BOOL printInsBin;
    BOOL printBranch;
    BOOL printImgName;
    BOOL printApiName;

    // anti-debug
    BOOL anti_isDebuggerPresent;
    BOOL anti_CheckRemoteDebuggerPresent;
    BOOL anti_SetInformationThread;
    BOOL anti_QueryInformationProcess;
    BOOL anti_UnhandledExceptionFilter;
    BOOL anti_QueryObject;
    BOOL anti_GlobalFlag;
    BOOL anti_HeapFlags;

    // method
    BOOL getOptions(int, char**);
    string getHelpForDisplay();
    string getSettingsForDisplay();
    string getDebugInfoForDisplay();

    BOOL isDumpRegTrigger(ADDRINT);
    BOOL isDumpMemTrigger(ADDRINT);
    struct MemAddrSize getDumpMem(ADDRINT);
};

