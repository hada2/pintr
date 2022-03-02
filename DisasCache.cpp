#include "DisasCache.hpp"
#include "Options.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;

DisasCache::DisasCache()
{

}

string DisasCache::getDissas(ADDRINT pc)
{
    return _disasCache[pc];
}

string DisasCache::getInsBin(ADDRINT pc)
{
    return _insBinCache[pc];
}

VOID DisasCache::setDisas(ADDRINT pc, string disas)
{
    _disasCache[pc] = disas;
}

VOID DisasCache::setInsBin(ADDRINT pc, string insBin)
{
    _insBinCache[pc] = insBin;
}

/*
VOID DisasCache::clearOldCache()
{

}
*/
