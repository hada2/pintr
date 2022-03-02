#pragma once

#include "pintr.hpp"

// to access disassembly from inside _AnalysisIns()
// https://reverseengineering.stackexchange.com/questions/12404/intel-pin-how-to-access-the-ins-object-from-inside-an-analysis-function

class DisasCache {
private:
    // member
    map<ADDRINT, string> _disasCache;
    map<ADDRINT, string> _insBinCache;

public:
    DisasCache();

    // method
    string getDissas(ADDRINT);
    string getInsBin(ADDRINT);

    VOID setDisas(ADDRINT, string);
    VOID setInsBin(ADDRINT, string);

    // TODO:
    // VOID clearOldCache();
};

