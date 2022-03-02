#include "Utils.hpp"
#include "Options.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;

Console::Console()
{

}

Console::~Console()
{
    if (_consoleFile) {
        _consoleFile->close();
    }
}

VOID Console::open(string filename)
{
    _consoleFile = new ofstream(filename.c_str());

    outInfo("Log File: " + gOpt.tracefileBaseName + "\n");
}

VOID Console::out(const char *str, UINT32 verboseLevel)
{
    PIN_GetLock(&gLockWrite, PIN_ThreadUid() + 1);

    if (gOpt.verboseLevel >= verboseLevel) {
        cout << str << flush;

        if (_consoleFile) {
            *_consoleFile << str << flush;
        }
    }

    PIN_ReleaseLock(&gLockWrite);
}

VOID Console::out(const string &str, UINT32 verboseLevel)
{
    out(str.c_str(), verboseLevel);
}

VOID Console::outInfo(const char *str, UINT32 verboseLevel)
{
    out(" [+] " + string(str), verboseLevel);
}

VOID Console::outInfo(const string &str, UINT32 verboseLevel)
{
    outInfo(str.c_str(), verboseLevel);
}

VOID Console::outErr(const char *str, UINT32 verboseLevel)
{
    out(" [-] " + string(str), verboseLevel);
}

VOID Console::outErr(const string &str, UINT32 verboseLevel)
{
    outErr(str.c_str(), verboseLevel);
}

TableForOutput::TableForOutput(const string &caption)
{
    _caption = caption;
}

VOID TableForOutput::clearTable()
{
    for (size_t i = 0; i < _table.size(); i++) {
        _table[i].clear();
    }

    _table.clear();
}

VOID TableForOutput::pushRow(const string column1, const string &column2, const string& column3, const string& column4)
{
    vector<string> w;

    w.push_back(column1);
    if (!column2.empty()) w.push_back(column2);
    if (!column3.empty()) w.push_back(column3);
    if (!column4.empty()) w.push_back(column4);

    _table.push_back(w);
}

string TableForOutput::genTableString()
{
    UINT32 rowLength = _getRowLength();
    UINT32 columnLength = _getColumnLength();

    vector<UINT32> w;

    for (UINT32 i = 0; i < columnLength; i++) {
        size_t maxLen = 0;

        for (UINT32 j = 0; j < rowLength; j++) {
            maxLen = max(_table[j][i].size(), maxLen);
        }

        maxLen += 1; // for space
        w.push_back(maxLen);
    }

    string str;

    if (!_caption.empty()) {
        str += _caption + "\n";
    }

    for (UINT32 i = 0; i < rowLength; i++) {
        for (UINT32 j = 0; j < columnLength; j++) {
            //string tmp = _table[i][j];

            if (j != columnLength - 1) {
                _table[i][j].resize(w[j], ' ');
            }

            str += _table[i][j];
        }

        str += "\n";
    }

    return str + "\n";
}

size_t TableForOutput::_getRowLength()
{
    return _table.size();
}

size_t TableForOutput::_getColumnLength()
{
    return _table[0].size();
}

VOID Console::close()
{
    _consoleFile->close();
}

UINT32 getNowTime() {

#ifdef __GNUC__
    struct timeval tv;

    gettimeofday(&tv, NULL);

	return tv.tv_sec*1000 + tv.tv_usec/1000;
#endif

#ifdef _MSC_VER
    WINDOWS::SYSTEMTIME st;
    WINDOWS::GetSystemTime(&st);

    unsigned int msec = st.wMilliseconds;
    unsigned int sec  = st.wSecond;
    unsigned int min  = st.wMinute;
    unsigned int hour = st.wHour;
    unsigned int day  = st.wDay;

    return day*24*60*60*1000 + hour*60*60*1000 + min*60*1000 + sec*1000 + msec;
#endif
}

vector<string> split(const string& s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }

    return elems;
}

UINT64 from_hex(const string& str) {
    UINT64 n;
    std::stringstream stm;
    stm << str;
    stm >> hex >> n;
    return n;
}

UINT64 from_string(const string& str) {
    UINT64 n;
    std::stringstream stm;
    stm << str;
    stm >> n;
    return n;
}


/*
VOID terminateCtrlC() {
    if (!WINDOWS::SetConsoleCtrlHandler(controlHandler, TRUE)) {
        outErr("ERROR: Could not set control handler\n");
        return;
    }
}

WINDOWS::BOOL WINAPI controlHandler(WINDOWS::DWORD signal) {
    if (signal == CTRL_C_EVENT)
        outInfo("Ctrl-C handled\n");

    return TRUE;
}
*/



