#pragma once

#include "pintr.hpp"

class RtnCache {
private:
    // member
    map<ADDRINT, string> _rtnCache;

public:
    RtnCache();

    // method
    string getRtn(ADDRINT);
    VOID setRtn(ADDRINT, string);

    // TODO:
    // VOID clearOldCache();
};


