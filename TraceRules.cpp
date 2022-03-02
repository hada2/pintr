#include "TraceRules.hpp"
#include "Options.hpp"
#include "Utils.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;
extern Console gConsole;

TraceRules::TraceRules() {

}

VOID TraceRules::appendDetectApiList(string name)
{
    _listDetectApiName.push_back(name);
}

VOID TraceRules::appendDetectMagicList(UINT64 magic)
{
    _listDetectMagicName.push_back(magic);
}

BOOL TraceRules::inDetectApiList(string name)
{
    if (count(_listDetectApiName.begin(), _listDetectApiName.end(), name)) {
        return true;
    }

    return false;
}

VOID TraceRules::appendHookImgList(string name)
{
    _listHookImg.push_back(name);
}

VOID TraceRules::appendHookRtnList(string name)
{
    _listHookRtn.push_back(name);
}

VOID TraceRules::appendHookRangeList(ADDRINT low, ADDRINT high)
{
    _mapHookRange[low] = high;
}

VOID TraceRules::appendSkipImgList(string name)
{
    _listSkipImg.push_back(name);
}

VOID TraceRules::appendSkipRtnList(string name)
{
    _listSkipRtn.push_back(name);
}

VOID TraceRules::appendSkipRangeList(ADDRINT lowAddr, ADDRINT highAddr)
{
    _mapSkipRange[lowAddr] = highAddr;
}

BOOL TraceRules::insIsInSkipImg(string imgName)
{
    if (_listHookImg.size() > 0) {
        if (count(_listHookImg.begin(), _listHookImg.end(), imgName) == 0)
            return true;
    } else {
        if (count(_listSkipImg.begin(), _listSkipImg.end(), imgName) > 0)
            return true;
    }

    return false;
}

BOOL TraceRules::insIsInSkipRtn(string rtnName)
{
    if (_listHookRtn.size() > 0) {
        if (count(_listHookRtn.begin(), _listHookRtn.end(), rtnName) == 0)
            return true;
    } else {
        if (count(_listSkipRtn.begin(), _listSkipRtn.end(), rtnName) > 0)
            return true;
    }

    return false;
}

BOOL TraceRules::insIsInSkipRange(ADDRINT pc)
{
    if (_mapHookRange.size() > 0) {
        for (auto iter = _mapHookRange.begin(); iter != _mapHookRange.end(); iter++) {
            ADDRINT low = iter->first;
            ADDRINT high = iter->second;

            if (low <= pc && pc <= high) {
                return false;
            }
        }

        return true;

    } else {
        for (auto iter = _mapSkipRange.begin(); iter != _mapSkipRange.end(); iter++) {
            ADDRINT low = iter->first;
            ADDRINT high = iter->second;

            if (low <= pc && pc <= high) {
                return true;
            }
        }

        return false;
    }
}

VOID TraceRules::appendTraceImgList(string imgName)
{
    _listTraceImg.push_back(imgName);
}

VOID TraceRules::appendTraceRtnList(string rtnName)
{
    _listTraceRtn.push_back(rtnName);
}

VOID TraceRules::appendTraceRangeList(ADDRINT addrLow, ADDRINT addrHigh)
{
    _mapTraceRange[addrLow] = addrHigh;
}

VOID TraceRules::appendFilterImgList(string imgName)
{
    _listFilterImg.push_back(imgName);
}

VOID TraceRules::appendFilterRtnList(string rtnName)
{
    _listFilterRtn.push_back(rtnName);
}

VOID TraceRules::appendFilterRangeList(ADDRINT addrLow, ADDRINT addrHigh)
{
    _mapFilterRange[addrLow] = addrHigh;
}

BOOL TraceRules::insIsInFilterImg(string imgName)
{
    if (_listTraceImg.size() > 0) {
        if (count(_listTraceImg.begin(), _listTraceImg.end(), imgName) == 0)
            return true;
    } else {
        if (count(_listFilterImg.begin(), _listFilterImg.end(), imgName) > 0)
            return true;
    }

    return false;
}

BOOL TraceRules::insIsInFilterRtn(string rtnName)
{
    if (_listTraceRtn.size() > 0) {
        if (count(_listTraceRtn.begin(), _listTraceRtn.end(), rtnName) == 0)
            return true;
    } else {
        if (count(_listFilterRtn.begin(), _listFilterRtn.end(), rtnName) > 0)
            return true;
    }

    return false;
}

BOOL TraceRules::insIsInFilterRange(ADDRINT pc)
{
    if (_mapTraceRange.size() > 0) {
        for (auto iter = _mapTraceRange.begin(); iter != _mapTraceRange.end(); iter++) {
            ADDRINT low = iter->first;
            ADDRINT high = iter->second;

            if (low <= pc && pc <= high) {
                return false;
            }
        }

        return true;

    } else {
        for (auto iter = _mapFilterRange.begin(); iter != _mapFilterRange.end(); iter++) {
            ADDRINT low = iter->first;
            ADDRINT high = iter->second;

            if (low <= pc && pc <= high) {
                return true;
            }
        }

        return false;
    }
}

vector<string> TraceRules::getSkipImgList()
{
    return _listSkipImg;
}

vector<string> TraceRules::getSkipRtnList()
{
    return _listSkipRtn;
}

vector<string> TraceRules::getHookImgList()
{
    return _listHookImg;
}

vector<string> TraceRules::getHookRtnList()
{
    return _listHookRtn;
}

vector<string> TraceRules::getFilterImgList()
{
    return _listFilterImg;
}

vector<string> TraceRules::getFilterRtnList()
{
    return _listFilterRtn;
}

vector<string> TraceRules::getTraceImgList()
{
    return _listTraceImg;
}

vector<string> TraceRules::getTraceRtnList()
{
    return _listTraceRtn;
}

string TraceRules::genHookRangeListSettingMsg()
{
    string str;

    for (auto iter = _mapHookRange.begin(); iter != _mapHookRange.end(); iter++) {
        ADDRINT low = iter->first;
        ADDRINT high = iter->second;
        str += to_hex(low) + "-" + to_hex(high) + " ";
    }

    return str;
}

string TraceRules::genSkipRangeListSettingMsg()
{
    string str;

    for (auto iter = _mapSkipRange.begin(); iter != _mapSkipRange.end(); iter++) {
        ADDRINT low = iter->first;
        ADDRINT high = iter->second;
        str += to_hex(low) + "-" + to_hex(high) + " ";
    }

    return str;
}

string TraceRules::genTraceRangeListSettingMsg()
{
    string str;

    for (auto iter = _mapTraceRange.begin(); iter != _mapTraceRange.end(); iter++) {
        ADDRINT low = iter->first;
        ADDRINT high = iter->second;
        str += to_hex(low) + "-" + to_hex(high) + " ";
    }

    return str;
}

string TraceRules::genFilterRangeListSettingMsg()
{
    string str;

    for (auto iter = _mapFilterRange.begin(); iter != _mapFilterRange.end(); iter++) {
        ADDRINT low = iter->first;
        ADDRINT high = iter->second;
        str += to_hex(low) + "-" + to_hex(high) + " ";
    }

    return str;
}

VOID TraceRules::clearAllLists()
{
    _listSkipImg.clear();
    _listHookImg.clear();
    _listSkipRtn.clear();
    _listHookRtn.clear();
    _mapSkipRange.clear();
    _mapHookRange.clear();

    _listFilterImg.clear();
    _listTraceImg.clear();
    _listFilterRtn.clear();
    _listTraceRtn.clear();
    _mapFilterRange.clear();
    _mapTraceRange.clear();
}


