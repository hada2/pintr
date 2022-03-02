#include "RtnCache.hpp"
#include "Options.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;

RtnCache::RtnCache()
{

}

string RtnCache::getRtn(ADDRINT pc)
{
    return _rtnCache[pc];
}

VOID RtnCache::setRtn(ADDRINT pc, string rtnName)
{
    _rtnCache[pc] = rtnName;
}

/*
VOID RtnCache::clearOldCache()
{

}
*/

