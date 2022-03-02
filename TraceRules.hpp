#pragma once

#include "pintr.hpp"

/*
  definition:
    skip   => not hooking
    hook   => hooking
    filter => hooking but not logging
    trace  => hooking and logging
*/

class TraceRules {
private:
    // detect
    vector<string> _listDetectApiName;
    vector<UINT64> _listDetectMagicName;

    // skip and hook
    vector<string> _listSkipImg;
    vector<string> _listHookImg;
    vector<string> _listSkipRtn;
    vector<string> _listHookRtn;
    map<ADDRINT, ADDRINT> _mapSkipRange;
    map<ADDRINT, ADDRINT> _mapHookRange;

    // filter and trace
    vector<string> _listFilterImg;
    vector<string> _listTraceImg;
    vector<string> _listFilterRtn;
    vector<string> _listTraceRtn;
    map<ADDRINT, ADDRINT> _mapFilterRange;
    map<ADDRINT, ADDRINT> _mapTraceRange;

public:
    TraceRules();

    // detect
    VOID appendDetectApiList(string);
    VOID appendDetectMagicList(UINT64);
    BOOL inDetectApiList(string);

    // hook and skip
    VOID appendHookImgList(string);
    VOID appendHookRtnList(string);
    VOID appendHookRangeList(ADDRINT, ADDRINT);
    VOID appendSkipImgList(string);
    VOID appendSkipRtnList(string);
    VOID appendSkipRangeList(ADDRINT, ADDRINT);
    BOOL insIsInSkipImg(string);
    BOOL insIsInSkipRtn(string);
    BOOL insIsInSkipRange(ADDRINT);

    // trace and filter
    VOID appendTraceImgList(string);
    VOID appendTraceRtnList(string);
    VOID appendTraceRangeList(ADDRINT, ADDRINT);
    VOID appendFilterImgList(string);
    VOID appendFilterRtnList(string);
    VOID appendFilterRangeList(ADDRINT, ADDRINT);
    BOOL insIsInFilterImg(string);
    BOOL insIsInFilterRtn(string);
    BOOL insIsInFilterRange(ADDRINT);

    vector<string> getSkipImgList();
    vector<string> getSkipRtnList();
    vector<string> getHookImgList();
    vector<string> getHookRtnList();
    vector<string> getFilterImgList();
    vector<string> getFilterRtnList();
    vector<string> getTraceImgList();
    vector<string> getTraceRtnList();

    string genHookRangeListSettingMsg();
    string genSkipRangeListSettingMsg();
    string genFilterRangeListSettingMsg();
    string genTraceRangeListSettingMsg();

    VOID clearAllLists();
};

