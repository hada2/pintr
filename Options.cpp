#include "Options.hpp"
#include "TraceRules.hpp"
#include "Utils.hpp"

extern Options gOpt;
extern Console gConsole;

/*
 * Knobs
 */

// Column
KNOB<BOOL> KnobSelectAll(                 KNOB_MODE_WRITEONCE, "pintool", "Sall", "0", "Select all columns");
KNOB<BOOL> KnobSelectTime(                KNOB_MODE_WRITEONCE, "pintool", "St", "0", "Select time");
KNOB<BOOL> KnobSelectLogCnt(              KNOB_MODE_WRITEONCE, "pintool", "Ss", "0", "Select log count");
KNOB<BOOL> KnobSelectInsBin(              KNOB_MODE_WRITEONCE, "pintool", "Sh", "0", "Select binary");
KNOB<BOOL> KnobSelectBranch(              KNOB_MODE_WRITEONCE, "pintool", "Sb", "0", "Select branch");
KNOB<BOOL> KnobSelectImgName(             KNOB_MODE_WRITEONCE, "pintool", "Si", "0", "Select image name");
KNOB<BOOL> KnobSelectRtnName(             KNOB_MODE_WRITEONCE, "pintool", "Sr", "0", "Select routine name");
KNOB<BOOL> KnobSelectRegFlag(             KNOB_MODE_WRITEONCE, "pintool", "Sf", "0", "Select registry flags");
KNOB<BOOL> KnobSelectRegGen(              KNOB_MODE_WRITEONCE, "pintool", "Sg", "0", "Select registry");
KNOB<BOOL> KnobSelectRegXMM(              KNOB_MODE_WRITEONCE, "pintool", "Sx", "0", "Select xmm");
KNOB<BOOL> KnobSelectRegYMM(              KNOB_MODE_WRITEONCE, "pintool", "Sy", "0", "Select ymm");

// Skip
KNOB<string> KnobSkipImg(                 KNOB_MODE_APPEND,    "pintool", "Ki", "", "Image name to skip");
KNOB<string> KnobSkipRtn(                 KNOB_MODE_APPEND,    "pintool", "Kr", "", "Routine name to skip");
KNOB<string> KnobSkipRange(               KNOB_MODE_APPEND,    "pintool", "Ka", "", "Address range to skip (e.g. 400000-408000)");

// Hook
KNOB<string> KnobHookImg(                 KNOB_MODE_APPEND,    "pintool", "Hi", "", "Image name to hook");
KNOB<string> KnobHookRtn(                 KNOB_MODE_APPEND,    "pintool", "Hr", "", "Routine name to hook");
KNOB<string> KnobHookRange(               KNOB_MODE_APPEND,    "pintool", "Ha", "", "Address range to hook (e.g. 400000-408000)");

// Filter
KNOB<string> KnobFilterImg(               KNOB_MODE_APPEND,    "pintool", "Fi", "", "Image name to filter");
KNOB<string> KnobFilterRtn(               KNOB_MODE_APPEND,    "pintool", "Fr", "", "Routine name to filter");
KNOB<string> KnobFilterRange(             KNOB_MODE_APPEND,    "pintool", "Fa", "", "Address range to filter (e.g. 400000-408000)");
KNOB<BOOL> KnobFilterBeforeEntryPoint(    KNOB_MODE_WRITEONCE, "pintool", "Fe", "0", "Filter before entry point (Start from entry point)");
KNOB<BOOL> KnobFilterBeforeMainImage(     KNOB_MODE_WRITEONCE, "pintool", "Fm", "0", "Filter before main image (Start from main image)");
KNOB<BOOL> KnobFilterBeforeStart(         KNOB_MODE_WRITEONCE, "pintool", "Fs", "0", "Filter before defined address (Start from defined address)");
KNOB<BOOL> KnobFilterAfterEnd(            KNOB_MODE_WRITEONCE, "pintool", "Fd", "0", "Filter after defined address (End in defined address)");
KNOB<BOOL> KnobFilterExceptApi(           KNOB_MODE_WRITEONCE, "pintool", "Fx", "0", "Filter all instructions (Log only API call)");

// Trace
KNOB<BOOL> KnobTraceAll(                  KNOB_MODE_WRITEONCE, "pintool", "a",  "0", "Trace all instruction");
KNOB<string> KnobTraceImg(                KNOB_MODE_APPEND,    "pintool", "Ti", "", "Image name to trace");
KNOB<string> KnobTraceRtn(                KNOB_MODE_APPEND,    "pintool", "Tr", "", "Routine name to trace");
KNOB<string> KnobTraceRange(              KNOB_MODE_APPEND,    "pintool", "Ta", "", "Address range to trace (e.g. 400000-408000)");
KNOB<BOOL> KnobTraceInsideRtn(            KNOB_MODE_WRITEONCE, "pintool", "Td", "0", "Trace inside routine");

// Exit
KNOB<BOOL> KnobExitInsLimitExceeded(      KNOB_MODE_WRITEONCE, "pintool", "Ei", "0", "Instructions count limit");
KNOB<BOOL> KnobExitTimeLimitExceeded(     KNOB_MODE_WRITEONCE, "pintool", "Et", "0", "Time limit");

// Bypass anti-debug
KNOB<BOOL> KnobBypassAntiDebugAll(        KNOB_MODE_WRITEONCE, "pintool", "Mall", "0", "Bypass all anti-debug");
KNOB<BOOL> KnobIsDebuggerPresent(         KNOB_MODE_WRITEONCE, "pintool", "Md", "0", "Bypass isDebuggerPresent technique");
KNOB<BOOL> KnobCheckRemoteDebuggerPresent(KNOB_MODE_WRITEONCE, "pintool", "Mr", "0", "Bypass CheckRemoteDebuggerPresent technique");
KNOB<BOOL> KnobSetInformationThread(      KNOB_MODE_WRITEONCE, "pintool", "Mi", "0", "Bypass SetInformationThread technique");
KNOB<BOOL> KnobQueryInformationProcess(   KNOB_MODE_WRITEONCE, "pintool", "Mq", "0", "Bypass QueryInformationProcess technique");
KNOB<BOOL> KnobUnhandledExceptionFilter(  KNOB_MODE_WRITEONCE, "pintool", "Mu", "0", "Bypass UnhandledExceptionFilter technique");
KNOB<BOOL> KnobQueryObject(               KNOB_MODE_WRITEONCE, "pintool", "Mo", "0", "Bypass QueryObject technique");
KNOB<BOOL> KnobGlobalFlag(                KNOB_MODE_WRITEONCE, "pintool", "Mg", "0", "Bypass GlobalFlag technique");
KNOB<BOOL> KnobHeapFlags(                 KNOB_MODE_WRITEONCE, "pintool", "Mh", "0", "Bypass HeapFlags technique");

// Detection
KNOB<BOOL> KnobCryptAnalysisAll(          KNOB_MODE_WRITEONCE, "pintool", "Dall", "0", "Detect all crypt elements");
KNOB<BOOL> KnobCryptMagicDetect(          KNOB_MODE_WRITEONCE, "pintool", "Dc", "0", "Detect crypt magic numbers");
KNOB<BOOL> KnobCryptApiDetect(            KNOB_MODE_WRITEONCE, "pintool", "Da", "0", "Detect crypt APIs");
KNOB<string> KnobRtnDetect(               KNOB_MODE_APPEND,    "pintool", "Dr", "", "Detect defined routine");
KNOB<string> KnobMagicDetect(             KNOB_MODE_APPEND,    "pintool", "Dm", "", "Detect defined magic number");

// Dump
KNOB<string> KnobDumpMem(                 KNOB_MODE_APPEND,    "pintool", "Om", "", "Output memory dump (e.g. 401000:7F000000:100)");
KNOB<string> KnobDumpReg(                 KNOB_MODE_APPEND,    "pintool", "Or", "", "Output registry (e.g. 401000)");

// Other
KNOB<BOOL> KnobQuiet(                     KNOB_MODE_WRITEONCE, "pintool", "q",  "0", "Quiet");
KNOB<BOOL> KnobVerbose(                   KNOB_MODE_WRITEONCE, "pintool", "v",  "0", "Verbose");
KNOB<BOOL> KnobVerbose2(                  KNOB_MODE_WRITEONCE, "pintool", "vv", "0", "Debug");
KNOB<BOOL> KnobOnlyMainThread(            KNOB_MODE_WRITEONCE, "pintool", "mt", "0", "Trace only main thread");
//KNOB<BOOL> KnobOnlyMainImage(KNOB_MODE_WRITEONCE, "pintool", "mi", "0", "Trace only main image");
KNOB<BOOL> KnobTraceFileName(             KNOB_MODE_WRITEONCE, "pintool", "o",  "0", "Trace filename");


Options::Options()
{
    tracefileBaseName = "trace-" + to_string(getNowTime() % 100000);

    isFilteringInsideApi = true;
    isFilteringInsideApiOrg = true;
    isStartingFromEntryPoint = false;
    isStartingFromMainImage = false;
    startAddress = 0;
    endAddress = 0;
    startAddressRangeLow = 0;
    startAddressRangeHigh = 0;
    traceExitTimeLimit = 0;
    traceExitInsLimit = 0;

    verboseLevel = VERBOSE_NORMAL;
    modeMainThreadOnly = false;
    modeApiOnly = false;
    modeMagicDetect = false;
    modeCryptApiDetect = false;

    printLogCount = false;
    printTime = false;
    printImgName = false;
    printApiName = false;
    printBranch = false;
    printRegGeneral = false;
    printRegFlag = false;
    printRegXmm = false;
    printRegYmm = false;
    printInsBin = false;

    anti_isDebuggerPresent = false;
    anti_CheckRemoteDebuggerPresent = false;
    anti_SetInformationThread = false;
    anti_QueryInformationProcess = false;
    anti_UnhandledExceptionFilter = false;
    anti_QueryObject = false;
    anti_GlobalFlag = false;
    anti_HeapFlags = false;
}

string Options::getHelpForDisplay()
{
    return KNOB_BASE::StringKnobSummary();
}

string Options::getSettingsForDisplay()
{
    string str;

    str += "Settings\n";

    // log columns
    {
        str += " [Columns]\n";

        string strColumns;
        strColumns += string(printLogCount ? "SeqNo " : "");
        strColumns += string(printTime ? "Time " : "");
        strColumns += string(printImgName ? "ImgName " : "");
        strColumns += string(printApiName ? "ApiName " : "");
        strColumns += string(printBranch ? "Branch " : "");
        strColumns += string(printRegFlag ? "RegFlag " : "");
        strColumns += string(printRegGeneral ? "RegGeneral " : "");
        strColumns += string(printRegXmm ? "RegXmm " : "");
        strColumns += string(printRegYmm ? "RegYmm " : "");
        strColumns += string(printInsBin ? "InsBin " : "");

        str += PADDING("") + string("Default ") + (strColumns.empty() ? "" : strColumns) + string("\n");
    }

    // trigger
    {
        str += " [Trigger]\n";

        string startAddr;
        if (startAddressRangeLow) startAddr += to_hex(startAddressRangeLow) + "-" + to_hex(startAddressRangeHigh);
        if (startAddress)         startAddr += to_hex(startAddress);
        str += PADDING("   Start") + string(startAddr.empty() ? "-" : startAddr) + string("\n");

        string endAddr;
        if (endAddress)           endAddr += to_hex(endAddress);
        str += PADDING("   End") + string(endAddr.empty() ? "-" : endAddr) + string("\n");
    }

    // rules
    {
        str += " [Rules]\n";

        string strSkip;
        for (auto imgName : traceRules.getSkipImgList()) strSkip += imgName + string(" ");
        for (auto rtnName : traceRules.getSkipRtnList()) strSkip += rtnName + string(" ");
        strSkip += traceRules.genSkipRangeListSettingMsg();
        str += PADDING("   Skip") + string(strSkip.empty() ? "-" : strSkip) + string("\n");

        string strHook;
        for (auto imgName : traceRules.getHookImgList()) strHook += imgName + string(" ");
        for (auto rtnName : traceRules.getHookRtnList()) strHook += rtnName + string(" ");
        strHook += traceRules.genHookRangeListSettingMsg();
        str += PADDING("   Hook") + string(strHook.empty() ? "-" : strHook) + string("\n");

        string strFilter;
        for (auto imgName : traceRules.getFilterImgList()) strFilter += imgName + string(" ");
        for (auto rtnName : traceRules.getFilterRtnList()) strFilter += rtnName + string(" ");
        strFilter += traceRules.genFilterRangeListSettingMsg();
        str += PADDING("   Filter") + string(strFilter.empty() ? "-" : strFilter) + string("\n");

        string strTrace;
        for (auto imgName : traceRules.getTraceImgList()) strTrace += imgName + string(" ");
        for (auto rtnName : traceRules.getTraceRtnList()) strTrace += rtnName + string(" ");
        strTrace += traceRules.genTraceRangeListSettingMsg();
        str += PADDING("   Trace") + string(strTrace.empty() ? "-" : strTrace) + string("\n");
    }

    // mode
    {
        str += " [Mode]\n";
        str += PADDING("   Log file (base)")    + tracefileBaseName + string("\n");
        str += PADDING("   Verbose level")      + to_string(verboseLevel) + string("\n");
        str += PADDING("   Only API")           + string(modeApiOnly        ? "Yes" : "No") + string("\n");
        str += PADDING("   Only main thread")   + string(modeMainThreadOnly ? "Yes" : "No") + string("\n");
        str += PADDING("   Detect crypt magic") + string(modeMagicDetect    ? "Yes" : "No") + string("\n");
        str += PADDING("   Detect crypt API")   + string(modeCryptApiDetect ? "Yes" : "No") + string("\n");
    }

    // dump
    {
        str += " [Dump]\n";

        string strDumpMem;
        for (auto iter = _mapDumpMem.begin(); iter != _mapDumpMem.end(); iter++) {
            ADDRINT insAddr = iter->first;
            struct MemAddrSize memInfo = iter->second;

            strDumpMem += to_hex(insAddr) + ":" + to_hex(memInfo.addr) + ":" + to_hex(memInfo.size) + string(" ");
        }
        str += PADDING("   Memory Dump") + string(strDumpMem.empty() ? "-" : strDumpMem) + string("\n");

        string strDumpReg;
        for (auto iter = _mapDumpReg.begin(); iter != _mapDumpReg.end(); iter++) {
            ADDRINT insAddr = iter->first;

            strDumpReg += to_hex(insAddr) + string(" ");
        }
        str += PADDING("   Registry Dump") + string(strDumpReg.empty() ? "-" : strDumpReg) + string("\n");
    }

    // Others
    {
        str += " [Others]\n";
        str += PADDING("   Time Limit") + (traceExitTimeLimit ? to_string(traceExitTimeLimit) + " ms\n" : "-\n");
        str += PADDING("   Ins Limit") + (traceExitInsLimit ? to_string(traceExitInsLimit) + " ins\n" : "-\n");
    }

    return str + "\n";
}

string Options::getDebugInfoForDisplay()
{
    string str;

    str += "Debug info\n";
    str += string("   isFilteringInsideApi     ") + (isFilteringInsideApi ? "Yes" : "No") + "\n";
    str += string("   isStartingFromEntryPoint ") + (isStartingFromEntryPoint ? "Yes" : "No") + "\n";
    str += string("   isStartingFromMainImage  ") + (isStartingFromMainImage ? "Yes" : "No") + "\n";
    str += string("   startAddress             ") + (to_hex(startAddress) + "\n");
    str += string("   endAddress               ") + (to_hex(endAddress) + "\n");
    str += string("   startAddressRangeLow     ") + (to_hex(startAddressRangeLow) + "\n");
    str += string("   startAddressRangeHigh    ") + (to_hex(startAddressRangeHigh) + "\n");

    return str + "\n";
}

BOOL Options::getOptions(int argc, char **argv)
{
    /*
     * Show columns
     */

    if (KnobSelectAll || KnobSelectLogCnt)  { printLogCount = true; }
    if (KnobSelectAll || KnobSelectTime)    { printTime = true; }
    if (KnobSelectAll || KnobSelectInsBin)  { printInsBin = true; }
    if (KnobSelectAll || KnobSelectBranch)  { printBranch = true; }
    if (KnobSelectAll || KnobSelectImgName) { printImgName = true; }
    if (KnobSelectAll || KnobSelectRtnName) { printApiName = true; }
    if (KnobSelectAll || KnobSelectRegFlag) { printRegFlag = true; }
    if (KnobSelectAll || KnobSelectRegGen)  { printRegGeneral = true; }
    if (KnobSelectAll || KnobSelectRegXMM)  { printRegXmm = true; }
    if (KnobSelectAll || KnobSelectRegYMM)  { printRegYmm = true; }

    /*
     * Skip
     */

    for (UINT32 i = 0; i < KnobSkipImg.NumberOfValues(); i++) {
        traceRules.appendSkipImgList(KnobSkipImg.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobSkipRtn.NumberOfValues(); i++) {
        traceRules.appendSkipRtnList(KnobSkipRtn.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobSkipRange.NumberOfValues(); i++) {
        vector<string> args = split(KnobSkipRange.ValueString(), '-');
        traceRules.appendSkipRangeList(from_hex(args[0]), from_hex(args[1]));
    }

    /*
     * Hook
     */
    
    for (UINT32 i = 0; i < KnobHookImg.NumberOfValues(); i++) {
        traceRules.appendHookImgList(KnobHookImg.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobHookRtn.NumberOfValues(); i++) {
        traceRules.appendHookRtnList(KnobHookRtn.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobHookRange.NumberOfValues(); i++) {
        vector<string> args = split(KnobHookRange.ValueString(), '-');
        traceRules.appendHookRangeList(from_hex(args[0]), from_hex(args[1]));
    }

    /*
     * Filter
     */

    if (KnobFilterBeforeEntryPoint) {
        isStartingFromEntryPoint = true;
        isFilteringInsideApi = false;
    }

    if (KnobFilterBeforeMainImage) {
        isStartingFromMainImage = true;
        isFilteringInsideApi = false;
    }

    if (KnobFilterBeforeStart) {
        startAddress = from_hex(KnobFilterBeforeStart.ValueString());
        isStartingFromEntryPoint = true;
        isFilteringInsideApi = false;
    }

    if (KnobFilterAfterEnd) {
        endAddress = from_hex(KnobFilterAfterEnd.ValueString());
    }

    for (UINT32 i = 0; i < KnobFilterImg.NumberOfValues(); i++) {
        traceRules.appendFilterImgList(KnobFilterImg.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobFilterRtn.NumberOfValues(); i++) {
        traceRules.appendFilterRtnList(KnobFilterRtn.ValueString(i));
    }

    if (!KnobFilterRange.ValueString().empty()) {
        vector<string> args = split(KnobFilterRange.ValueString(), '-');
        traceRules.appendFilterRangeList(from_hex(args[0]), from_hex(args[1]));
    }

    if (KnobFilterExceptApi) {
        modeApiOnly = true;
    }

    /*
     * Trace
     */

    if (KnobTraceInsideRtn) {
        isFilteringInsideApi = false;
        isFilteringInsideApiOrg = false;
    }

    for (UINT32 i = 0; i < KnobTraceImg.NumberOfValues(); i++) {
        traceRules.appendTraceImgList(KnobTraceImg.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobTraceRtn.NumberOfValues(); i++) {
        traceRules.appendTraceRtnList(KnobTraceRtn.ValueString(i));
    }

    if (!KnobTraceRange.ValueString().empty()) {
        vector<string> args = split(KnobTraceRange.ValueString(), '-');
        traceRules.appendTraceRangeList(from_hex(args[0]), from_hex(args[1]));
    }

    /*
     * Exit
     */

    if (KnobExitInsLimitExceeded) {
        traceExitInsLimit = from_string(KnobExitInsLimitExceeded.ValueString());
    }

    if (KnobExitTimeLimitExceeded) {
        traceExitTimeLimit = from_string(KnobExitTimeLimitExceeded.ValueString());
    }

    /*
     * Bypass anti-debug
     */

    if (KnobBypassAntiDebugAll || KnobIsDebuggerPresent) { anti_isDebuggerPresent = true; }
    if (KnobBypassAntiDebugAll || KnobCheckRemoteDebuggerPresent) { anti_CheckRemoteDebuggerPresent = true; }
    if (KnobBypassAntiDebugAll || KnobSetInformationThread) { anti_SetInformationThread = true; }
    if (KnobBypassAntiDebugAll || KnobQueryInformationProcess) { anti_QueryInformationProcess = true; }
    if (KnobBypassAntiDebugAll || KnobUnhandledExceptionFilter) { anti_UnhandledExceptionFilter = true; } // TODO
    if (KnobBypassAntiDebugAll || KnobQueryObject) { anti_QueryObject = true; }
    if (KnobBypassAntiDebugAll || KnobGlobalFlag) { anti_GlobalFlag = true; }
    if (KnobBypassAntiDebugAll || KnobHeapFlags) { anti_HeapFlags = true; }

    /*
     * Detect
     */
    
    // Crypt analysis
    if (KnobCryptAnalysisAll || KnobCryptMagicDetect)   { modeMagicDetect = true; }
    if (KnobCryptAnalysisAll || KnobCryptApiDetect)     { modeCryptApiDetect = true; }

    // User-defined
    for (UINT32 i = 0; i < KnobRtnDetect.NumberOfValues(); i++) {
        traceRules.appendDetectApiList(KnobRtnDetect.ValueString(i));
    }

    for (UINT32 i = 0; i < KnobMagicDetect.NumberOfValues(); i++) {
        traceRules.appendDetectMagicList(from_hex(KnobMagicDetect.ValueString(i)));
    }

    /*
     * Dump
     */

    for (UINT32 i = 0; i < KnobDumpMem.NumberOfValues(); i++) {
        vector<string> args = split(KnobDumpMem.ValueString(i), ':');
        ADDRINT insAddr = from_hex(args[0]);
        ADDRINT memAddr = from_hex(args[1]);
        size_t  memSize = from_hex(args[2]);
        _mapDumpMem[insAddr].addr = memAddr;
        _mapDumpMem[insAddr].size = memSize;
    }

    for (UINT32 i = 0; i < KnobDumpReg.NumberOfValues(); i++) {
        ADDRINT insAddr = from_hex(KnobDumpReg.ValueString(i));
        _mapDumpReg[insAddr] = 1;
    }

    /*
     * Others
     */

    if (KnobQuiet)          { verboseLevel = VERBOSE_QUIET; }
    if (KnobVerbose)        { verboseLevel = VERBOSE_INFO; }
    if (KnobVerbose2)       { verboseLevel = VERBOSE_DEBUG; }
    if (KnobOnlyMainThread) { modeMainThreadOnly = true; }
    if (KnobTraceFileName)  { tracefileBaseName = KnobTraceFileName.ValueString(); }

    if (KnobTraceAll) {
        traceRules.clearAllLists();
        isFilteringInsideApi = false;
        isFilteringInsideApiOrg = false;
    }

    return true;
}

BOOL Options::isDumpMemTrigger(ADDRINT pc)
{
    return _mapDumpMem.count(pc) > 0;
}

struct MemAddrSize Options::getDumpMem(ADDRINT pc)
{
    return _mapDumpMem[pc];
}

BOOL Options::isDumpRegTrigger(ADDRINT pc)
{
    return _mapDumpReg.count(pc) > 0;
}

