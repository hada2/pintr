#include "Tracer.hpp"
#include "Magic.hpp"
#include "Options.hpp"
#include "Utils.hpp"

extern PIN_LOCK gLockWrite;
extern Options gOpt;
extern Console gConsole;

// static member
ThreadInfoManager Tracer::_tiManager;
GlobalMagicTable Tracer::_magic;
DisasCache Tracer::_disasCache;
RtnCache Tracer::_rtnCache;
UINT32 Tracer::_initTime = 0;
UINT32 Tracer::_lastRefreshTime = 0;
BOOL Tracer::_firstInstruction = true;
string Tracer::_osVersion;

Tracer::Tracer()
{
    _initTime = getNowTime();
    _osVersion = _getOsVersion();
}

UINT32 Tracer::_getRunningTime()
{
    return getNowTime() - _initTime;
}

BOOL Tracer::_checkRefreshInterval(UINT32 intervalMs)
{
    if (getNowTime() - _lastRefreshTime > intervalMs) {
        _lastRefreshTime = getNowTime();
        return true;
    }

    return false;
}

BOOL Tracer::_isFirstInstruction()
{
    if (_firstInstruction) {
        _firstInstruction = false;
        return true;
    }

    return false;
}

string Tracer::_genRegXmmValue(const REG reg, const CONTEXT *ctx)
{
    UINT64 value1 = 0, value2 = 0;
    _parseXmm(&value1, &value2, reg, ctx);
    return to_HEX(value1) + ":" + to_HEX(value2);
}

string Tracer::_genRegYmmValue(const REG reg, const CONTEXT *ctx)
{
    UINT64 value1, value2, value3, value4;
    _parseYmm(&value4, &value3, &value2, &value1, reg, ctx);
    return to_HEX(value1) + ":" + to_HEX(value2) + ":" + to_HEX(value3) + ":" + to_HEX(value4);
}

VOID Tracer::_parseRegFlag(UINT64& readFlags, UINT64& writtenFlags, ADDRINT pc)
{
    // 64 bit
    static const xed_state_t dstate = { XED_MACHINE_MODE_LONG_64, XED_ADDRESS_WIDTH_64b };

    // 32 bit
    //static const xed_state_t dstate = { XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b };

    xed_decoded_inst_t xedd;
    xed_decoded_inst_zero_set_mode(&xedd, &dstate);

    xed_error_enum_t xed_code = xed_decode(&xedd, reinterpret_cast<UINT8*>(pc), 15);
    BOOL xed_ok = (xed_code == XED_ERROR_NONE);

    if (xed_ok) {
        xed_encoder_request_init_from_decode(&xedd);

        if (xed_decoded_inst_uses_rflags(&xedd)) {
            const xed_simple_flag_t* rfi = xed_decoded_inst_get_rflags_info(&xedd);

            if (xed_simple_flag_reads_flags(rfi)) {
                const xed_flag_set_t* read_set = xed_simple_flag_get_read_flag_set(rfi);
                readFlags = xed_flag_set_mask(read_set);
            }

            if (xed_simple_flag_writes_flags(rfi)) {
                const xed_flag_set_t* written_set = xed_simple_flag_get_written_flag_set(rfi);
                writtenFlags = xed_flag_set_mask(written_set);
            }
        }
    }
}

VOID Tracer::_parseXmm(UINT64* low, UINT64* high, const REG reg, const CONTEXT* ctx)
{
    UINT64 tmp[4];

    PIN_GetContextRegval(ctx, reg, reinterpret_cast<UINT8*>(&tmp));

    *low = tmp[0];
    *high = tmp[1];
}

VOID Tracer::_parseYmm(UINT64* value1, UINT64* value2, UINT64* value3, UINT64* value4, const REG reg, const CONTEXT* ctx)
{
    UINT64 tmp[8];

    PIN_GetContextRegval(ctx, reg, reinterpret_cast<UINT8*>(&tmp));

    *value1 = tmp[0];
    *value2 = tmp[1];
    *value3 = tmp[2];
    *value4 = tmp[3];
}

string Tracer::_genRegGeneral(const CONTEXT *ctx)
{
    string str;

    str += "RAX=" + to_HEX(PIN_GetContextReg(ctx, REG_RAX)) + ",";
    str += "RBX=" + to_HEX(PIN_GetContextReg(ctx, REG_RBX)) + ",";
    str += "RCX=" + to_HEX(PIN_GetContextReg(ctx, REG_RCX)) + ",";
    str += "RDX=" + to_HEX(PIN_GetContextReg(ctx, REG_RDX)) + ",";
    str += "R8="  + to_HEX(PIN_GetContextReg(ctx, REG_R8))  + ",";
    str += "R9="  + to_HEX(PIN_GetContextReg(ctx, REG_R9))  + ",";
    str += "R10=" + to_HEX(PIN_GetContextReg(ctx, REG_R10)) + ",";
    str += "R11=" + to_HEX(PIN_GetContextReg(ctx, REG_R11)) + ",";
    str += "R12=" + to_HEX(PIN_GetContextReg(ctx, REG_R12)) + ",";
    str += "R13=" + to_HEX(PIN_GetContextReg(ctx, REG_R13)) + ",";
    str += "R14=" + to_HEX(PIN_GetContextReg(ctx, REG_R14)) + ",";
    str += "R15=" + to_HEX(PIN_GetContextReg(ctx, REG_R15)) + ",";
    str += "RBP=" + to_HEX(PIN_GetContextReg(ctx, REG_RBP)) + ",";
    str += "RSP=" + to_HEX(PIN_GetContextReg(ctx, REG_RSP)) + ",";
    str += "RDI=" + to_HEX(PIN_GetContextReg(ctx, REG_RDI)) + ",";
    str += "RSI=" + to_HEX(PIN_GetContextReg(ctx, REG_RSI));

    return str;
}

string Tracer::_genRegXmm(const CONTEXT *ctx)
{
    string str;

    str += "XMM0="  + _genRegXmmValue(REG_XMM0, ctx)  + ",";
    str += "XMM1="  + _genRegXmmValue(REG_XMM1, ctx)  + ",";
    str += "XMM2="  + _genRegXmmValue(REG_XMM2, ctx)  + ",";
    str += "XMM3="  + _genRegXmmValue(REG_XMM3, ctx)  + ",";
    str += "XMM4="  + _genRegXmmValue(REG_XMM4, ctx)  + ",";
    str += "XMM5="  + _genRegXmmValue(REG_XMM5, ctx)  + ",";
    str += "XMM6="  + _genRegXmmValue(REG_XMM6, ctx)  + ",";
    str += "XMM7="  + _genRegXmmValue(REG_XMM7, ctx)  + ",";
    str += "XMM8="  + _genRegXmmValue(REG_XMM8, ctx)  + ",";
    str += "XMM9="  + _genRegXmmValue(REG_XMM9, ctx)  + ",";
    str += "XMM10=" + _genRegXmmValue(REG_XMM10, ctx) + ",";
    str += "XMM11=" + _genRegXmmValue(REG_XMM11, ctx) + ",";
    str += "XMM12=" + _genRegXmmValue(REG_XMM12, ctx) + ",";
    str += "XMM13=" + _genRegXmmValue(REG_XMM13, ctx) + ",";
    str += "XMM14=" + _genRegXmmValue(REG_XMM14, ctx) + ",";
    str += "XMM15=" + _genRegXmmValue(REG_XMM15, ctx);

    return str;
}

string Tracer::_genRegYmm(const CONTEXT *ctx)
{
    string str;

    str += "YMM0="  + _genRegYmmValue(REG_YMM0, ctx)  + ",";
    str += "YMM1="  + _genRegYmmValue(REG_YMM1, ctx)  + ",";
    str += "YMM2="  + _genRegYmmValue(REG_YMM2, ctx)  + ",";
    str += "YMM3="  + _genRegYmmValue(REG_YMM3, ctx)  + ",";
    str += "YMM4="  + _genRegYmmValue(REG_YMM4, ctx)  + ",";
    str += "YMM5="  + _genRegYmmValue(REG_YMM5, ctx)  + ",";
    str += "YMM6="  + _genRegYmmValue(REG_YMM6, ctx)  + ",";
    str += "YMM7="  + _genRegYmmValue(REG_YMM7, ctx)  + ",";
    str += "YMM8="  + _genRegYmmValue(REG_YMM8, ctx)  + ",";
    str += "YMM9="  + _genRegYmmValue(REG_YMM9, ctx)  + ",";
    str += "YMM10=" + _genRegYmmValue(REG_YMM10, ctx) + ",";
    str += "YMM11=" + _genRegYmmValue(REG_YMM11, ctx) + ",";
    str += "YMM12=" + _genRegYmmValue(REG_YMM12, ctx) + ",";
    str += "YMM13=" + _genRegYmmValue(REG_YMM13, ctx) + ",";
    str += "YMM14=" + _genRegYmmValue(REG_YMM14, ctx) + ",";
    str += "YMM15=" + _genRegYmmValue(REG_YMM15, ctx);

    return str;
}

BOOL Tracer::_isFiltered(ADDRINT pc)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.getTiTracingEnabled(tid))
        return true;

    if (_tiManager.filterTiUntilReturnAddr(tid, pc))
        return true;

    if (gOpt.modeFilterExceptJump) {
        ADDRINT lastPc = _tiManager.getTiLastPcHistory(tid);

        if (_disasCache.getDissas(pc).find("j") != 0 &&
            _disasCache.getDissas(lastPc).find("j") != 0) {
            return true;
        }
    }

    if (gOpt.modeApiOnly) {
        if (!contain(_disasCache.getDissas(pc), "call")) {
            return true;
        }
    }

    string imgName = _tiManager.isInTiLastImgRange(tid, pc) ?
        _tiManager.getTiLastImgName(tid) : _getImgName(pc);

    string rtnName = _tiManager.isInTiLastRtnRange(tid, pc) ?
        _tiManager.getTiLastRtnName(tid) : _getRtnName(pc);

    if (gOpt.traceRules.insIsInFilterImg(imgName)) return true;
    if (gOpt.traceRules.insIsInFilterRtn(rtnName)) return true;
    if (gOpt.traceRules.insIsInFilterRange(pc))    return true;

    return false;
}

VOID Tracer::_AnalysisIns(ADDRINT pc, CONTEXT *ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    // output previous line
    _tiManager.writeTiLine(tid);
    _tiManager.clearTiLine(tid);

    _tiManager.pushTiPcHistory(tid, pc);
    _tiManager.incTiInsCount(tid);

    _enableTracingIfStart(pc);
    _disableTracingIfEnd(pc);

    // filter
    if (_isFiltered(pc))
        return;

    _tiManager.incTiLogCount(tid);

    // Reopen log file
    if (_checkRefreshInterval(REFRESH_INTERVAL_MS)) {
        for (PIN_THREAD_UID tid : _tiManager.getTidList()) {
            _tiManager.refreshTiTracefile(tid);
        }
    }

    // memory dump
    if (gOpt.isDumpMemTrigger(pc)) {
        _dumpMemory(pc, gOpt.getDumpMem(pc), ctx);
    }

    // set basic info
    string prefix, suffix; // reserved
    UINT32 logCount = _tiManager.getTiLogCount(tid);
    _tiManager.setTiBasicInfo(tid, prefix, suffix, logCount, _getRunningTime());

    // set disas
    _tiManager.setTiDisas(tid, pc, _disasCache.getDissas(pc));
    _tiManager.setTiInsBin(tid, _disasCache.getInsBin(pc));

    // set image name
    if (gOpt.printImgName) {
        string imgName = _tiManager.isInTiLastImgRange(tid, pc) ?
                         _tiManager.getTiLastImgName(tid) : _getImgName(pc);
        _tiManager.setTiImgName(tid, imgName);
    }

    // set API name
    if (gOpt.printApiName) {
        string apiName = _tiManager.isInTiLastRtnRange(tid, pc) ?
                         _tiManager.getTiLastRtnName(tid) : _getRtnName(pc);
        _tiManager.setTiApiName(tid, apiName);
    }

    // set reg flags
    if (gOpt.printRegFlag) {
        UINT64 regFlagsRead = 0;
        UINT64 regFlagsWrite = 0;

        _parseRegFlag(regFlagsRead, regFlagsWrite, pc);
        _tiManager.setTiRegFlagRead(tid, _genRegFlagColumn(regFlagsRead));
        _tiManager.setTiRegFlagWrite(tid, _genRegFlagColumn(regFlagsWrite));
    }

    _RewriteBeforeIns(pc, ctx);
}

VOID Tracer::_AnalysisInsAfter(ADDRINT pc,
                               CONTEXT *ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    // set all reg data
    string allRegs;
    if (gOpt.printRegGeneral) { allRegs += _genRegGeneral(ctx); }
    if (gOpt.printRegXmm)     { allRegs += (allRegs.empty() ? "" : "_") + _genRegXmm(ctx); }
    if (gOpt.printRegYmm)     { allRegs += (allRegs.empty() ? "" : "_") + _genRegYmm(ctx); }
    _tiManager.setTiAllRegs(tid, allRegs);

    // print all reg data
    if (gOpt.isDumpRegTrigger(pc)) {
        _dumpReg(pc, allRegs);
    }

    // rewrite
    _RewriteAfterIns(pc, ctx);
}

VOID Tracer::_AnalysisInsBlanch(ADDRINT pc,
                                CONTEXT* ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    _tiManager.setTiBranch(tid, true);
}

VOID Tracer::_AnalysisRegisterRead(ADDRINT pc,
                                   UINT64 registersUsed1,
                                   UINT64 registersUsed2,
                                   CONTEXT *ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    _tiManager.setTiRegRead(tid, _genRegColumn(registersUsed1, registersUsed2, ctx));
}

VOID Tracer::_AnalysisRegisterWrite(ADDRINT pc,
                                    UINT64 registersUsed1,
                                    UINT64 registersUsed2,
                                    CONTEXT *ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    _tiManager.setTiRegWrite(tid, _genRegColumn(registersUsed1, registersUsed2, ctx));
}

VOID Tracer::_AnalysisMemoryRead(ADDRINT pc,
                                 ADDRINT memAddr,
                                 size_t memSize,
                                 CONTEXT* ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    if (gOpt.modeMagicDetect) {
        UINT64 value = 0;
        string cryptName;

        PIN_SafeCopy(&value, (VOID*)memAddr, memSize);
        cryptName = _magic.isInMagicNumberMap(value);

        if (!cryptName.empty()) {
            gConsole.outInfo("Crypto found (" + cryptName + "): " + to_hex(pc) + "\n");
        }
    }

    _tiManager.setTiMemRead(tid, _genMemColumn(memAddr, memSize));
}

VOID Tracer::_AnalysisMemoryWriteBefore(ADDRINT memAddr, size_t memSize)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    _tiManager.setTiMemWriteAddr(tid, memAddr);
    _tiManager.setTiMemWriteSize(tid, memSize);
}

VOID Tracer::_AnalysisMemoryWrite(ADDRINT pc)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    ADDRINT memAddr = _tiManager.getTiMemWriteAddr(tid);
    size_t memSize = _tiManager.getTiMemWriteSize(tid);

    _tiManager.setTiMemWrite(tid, _genMemColumn(memAddr, memSize));
}

string Tracer::_genMemColumn(const ADDRINT memAddr, const size_t memSize)
{
    string str;

    str += to_hex(memAddr) + "_" + std::to_string(memSize) + "_";

    switch (memSize) {
    case 1:
    case 2:
    case 4:
    case 8:
        {
            UINT64 value = 0;

            PIN_SafeCopy(&value, (VOID*)memAddr, memSize);
            str += to_hex(value);

            break;
        }
    case 16:
        {
            UINT64 value1 = 0, value2 = 0;

            PIN_SafeCopy(&value1, (VOID*)(memAddr + 0), 8);
            PIN_SafeCopy(&value2, (VOID*)(memAddr + 8), 8);
            str += to_hex(value1) + ":" + to_hex(value2);

            break;
        }
    case 32:
        {
            UINT64 value1 = 0, value2 = 0, value3 = 0, value4 = 0;

            PIN_SafeCopy(&value1, (VOID*)(memAddr + 0), 8);
            PIN_SafeCopy(&value2, (VOID*)(memAddr + 8), 8);
            PIN_SafeCopy(&value3, (VOID*)(memAddr + 16), 8);
            PIN_SafeCopy(&value4, (VOID*)(memAddr + 24), 8);
            str += to_hex(value1) + ":" + to_hex(value2) + ":" + to_hex(value3) + ":" + to_hex(value4);

            break;
        }
    default:
        // TODO: debug
        str += "Unsupported memSize";
        break;
    }

    return str;
}

string Tracer::_genRegColumn(const UINT64 registerDef1, const UINT64 registerDef2, const CONTEXT *ctx)
{
    UINT64 value = 0;
    UINT64 value1 = 0, value2 = 0, value3 = 0, value4 = 0;

    string str;

    if (registerDef1 & FLAG_REG_RAX) {
        value = PIN_GetContextReg(ctx, REG_RAX);

        if (registerDef2 & FLAG_REG_R) {
            str += "rax_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "eax_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "ax_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_H) {
            value &= 0x0000FF00;
            str += "ah_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "al_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RBX) {
        value = PIN_GetContextReg(ctx, REG_RBX);

        if (registerDef2 & FLAG_REG_R) {
            str += "rbx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "ebx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "bx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_H) {
            value &= 0x0000FF00;
            str += "bh_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "bl_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RCX) {
        value = PIN_GetContextReg(ctx, REG_RCX);

        if (registerDef2 & FLAG_REG_R) {
            str += "rcx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "ecx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "cx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_H) {
            value &= 0x0000FF00;
            str += "ch_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "cl_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RDX) {
        value = PIN_GetContextReg(ctx, REG_RDX);

        if (registerDef2 & FLAG_REG_R) {
            str += "rdx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "edx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "dx_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_H) {
            value &= 0x0000FF00;
            str += "dh_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "dl_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RSI) {
        value = PIN_GetContextReg(ctx, REG_RSI);

        if (registerDef2 & FLAG_REG_R) {
            str += "rsi_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "esi_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "si_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "sil_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RDI) {
        value = PIN_GetContextReg(ctx, REG_RDI);

        if (registerDef2 & FLAG_REG_R) {
            str += "rdi_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "edi_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "di_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "dil_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RBP) {
        value = PIN_GetContextReg(ctx, REG_RBP);

        if (registerDef2 & FLAG_REG_R) {
            str += "rbp_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "ebp_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "bp_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "bpl_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_RSP) {
        value = PIN_GetContextReg(ctx, REG_RSP);

        if (registerDef2 & FLAG_REG_R) {
            str += "rsp_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "esp_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "sp_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "spl_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R8) {
        value = PIN_GetContextReg(ctx, REG_R8);

        if (registerDef2 & FLAG_REG_R) {
            str += "r8_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r8d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r8w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r8b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R9) {
        value = PIN_GetContextReg(ctx, REG_R9);

        if (registerDef2 & FLAG_REG_R) {
            str += "r9_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r9d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r9w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r9b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R10) {
        value = PIN_GetContextReg(ctx, REG_R10);

        if (registerDef2 & FLAG_REG_R) {
            str += "r10_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r10d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r10w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r10b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R11) {
        value = PIN_GetContextReg(ctx, REG_R11);

        if (registerDef2 & FLAG_REG_R) {
            str += "r11_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r11d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r11w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r11b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R12) {
        value = PIN_GetContextReg(ctx, REG_R12);

        if (registerDef2 & FLAG_REG_R) {
            str += "r12_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r12d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r12w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r12b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R13) {
        value = PIN_GetContextReg(ctx, REG_R13);

        if (registerDef2 & FLAG_REG_R) {
            str += "r13_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r13d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r13w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r13b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R14) {
        value = PIN_GetContextReg(ctx, REG_R14);

        if (registerDef2 & FLAG_REG_R) {
            str += "r14_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r14d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r14w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r14b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_R15) {
        value = PIN_GetContextReg(ctx, REG_R15);

        if (registerDef2 & FLAG_REG_R) {
            str += "r15_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_E) {
            value &= 0xFFFFFFFF;
            str += "r15d_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_X) {
            value &= 0x0000FFFF;
            str += "r15w_" + to_hex(value);
        } else if (registerDef2 & FLAG_REG_L) {
            value &= 0x000000FF;
            str += "r15b_" + to_hex(value);
        }

    } else if (registerDef1 & FLAG_REG_XMM0) {
        _parseXmm(&value1, &value2, REG_XMM0, ctx);
        str += "xmm0_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM1) {
        _parseXmm(&value1, &value2, REG_XMM1, ctx);
        str += "xmm1_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM2) {
        _parseXmm(&value1, &value2, REG_XMM2, ctx);
        str += "xmm2_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM3) {
        _parseXmm(&value1, &value2, REG_XMM3, ctx);
        str += "xmm3_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM4) {
        _parseXmm(&value1, &value2, REG_XMM4, ctx);
        str += "xmm4_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM5) {
        _parseXmm(&value1, &value2, REG_XMM5, ctx);
        str += "xmm5_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM6) {
        _parseXmm(&value1, &value2, REG_XMM6, ctx);
        str += "xmm6_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM7) {
        _parseXmm(&value1, &value2, REG_XMM7, ctx);
        str += "xmm7_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM8) {
        _parseXmm(&value1, &value2, REG_XMM8, ctx);
        str += "xmm8_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM9) {
        _parseXmm(&value1, &value2, REG_XMM9, ctx);
        str += "xmm9_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM10) {
        _parseXmm(&value1, &value2, REG_XMM10, ctx);
        str += "xmm10_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM11) {
        _parseXmm(&value1, &value2, REG_XMM11, ctx);
        str += "xmm11_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM12) {
        _parseXmm(&value1, &value2, REG_XMM12, ctx);
        str += "xmm12_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM13) {
        _parseXmm(&value1, &value2, REG_XMM13, ctx);
        str += "xmm13_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM14) {
        _parseXmm(&value1, &value2, REG_XMM14, ctx);
        str += "xmm14_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_XMM15) {
        _parseXmm(&value1, &value2, REG_XMM15, ctx);
        str += "xmm15_" + to_hex(value1) + ":" + to_hex(value2);

    } else if (registerDef1 & FLAG_REG_YMM0) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM0, ctx);
        str += "ymm0_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM1) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM1, ctx);
        str += "ymm1_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM2) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM2, ctx);
        str += "ymm2_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM3) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM3, ctx);
        str += "ymm3_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM4) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM4, ctx);
        str += "ymm4_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM5) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM5, ctx);
        str += "ymm5_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM6) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM6, ctx);
        str += "ymm6_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM7) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM7, ctx);
        str += "ymm7_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM8) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM8, ctx);
        str += "ymm8_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM9) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM9, ctx);
        str += "ymm9_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM10) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM10, ctx);
        str += "ymm10_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM11) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM11, ctx);
        str += "ymm11_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM12) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM12, ctx);
        str += "ymm12_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM13) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM13, ctx);
        str += "ymm13_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM14) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM14, ctx);
        str += "ymm14_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    } else if (registerDef1 & FLAG_REG_YMM15) {
        _parseYmm(&value1, &value2, &value3, &value4, REG_YMM15, ctx);
        str += "ymm15_" + to_hex(value4) + ":" + to_hex(value3) + ":" + to_hex(value2) + ":" + to_hex(value1);

    }

    return str;
}

VOID Tracer::_setupRegValue(UINT64 &registerDef1, UINT64 &registerDef2, const string registerName)
{
    registerDef1 = 0;
    registerDef2 = 0;

    if (registerName.compare(string("rax")) == 0) {
        registerDef1 |= FLAG_REG_RAX;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("eax")) == 0) {
        registerDef1 |= FLAG_REG_RAX;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("ax")) == 0) {
        registerDef1 |= FLAG_REG_RAX;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("ah")) == 0) {
        registerDef1 |= FLAG_REG_RAX;
        registerDef2 |= FLAG_REG_H;
    } else if (registerName.compare(string("al")) == 0) {
        registerDef1 |= FLAG_REG_RAX;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rbx")) == 0) {
        registerDef1 |= FLAG_REG_RBX;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("ebx")) == 0) {
        registerDef1 |= FLAG_REG_RBX;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("bx")) == 0) {
        registerDef1 |= FLAG_REG_RBX;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("bh")) == 0) {
        registerDef1 |= FLAG_REG_RBX;
        registerDef2 |= FLAG_REG_H;
    } else if (registerName.compare(string("bl")) == 0) {
        registerDef1 |= FLAG_REG_RBX;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rcx")) == 0) {
        registerDef1 |= FLAG_REG_RCX;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("ecx")) == 0) {
        registerDef1 |= FLAG_REG_RCX;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("cx")) == 0) {
        registerDef1 |= FLAG_REG_RCX;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("ch")) == 0) {
        registerDef1 |= FLAG_REG_RCX;
        registerDef2 |= FLAG_REG_H;
    } else if (registerName.compare(string("cl")) == 0) {
        registerDef1 |= FLAG_REG_RCX;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rdx")) == 0) {
        registerDef1 |= FLAG_REG_RDX;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("edx")) == 0) {
        registerDef1 |= FLAG_REG_RDX;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("dx")) == 0) {
        registerDef1 |= FLAG_REG_RDX;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("dh")) == 0) {
        registerDef1 |= FLAG_REG_RDX;
        registerDef2 |= FLAG_REG_H;
    } else if (registerName.compare(string("dl")) == 0) {
        registerDef1 |= FLAG_REG_RDX;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rsi")) == 0) {
        registerDef1 |= FLAG_REG_RSI;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("esi")) == 0) {
        registerDef1 |= FLAG_REG_RSI;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("si")) == 0) {
        registerDef1 |= FLAG_REG_RSI;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("sil")) == 0) {
        registerDef1 |= FLAG_REG_RSI;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rdi")) == 0) {
        registerDef1 |= FLAG_REG_RDI;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("edi")) == 0) {
        registerDef1 |= FLAG_REG_RDI;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("di")) == 0) {
        registerDef1 |= FLAG_REG_RDI;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("dil")) == 0) {
        registerDef1 |= FLAG_REG_RDI;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rbp")) == 0) {
        registerDef1 |= FLAG_REG_RBP;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("ebp")) == 0) {
        registerDef1 |= FLAG_REG_RBP;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("bp")) == 0) {
        registerDef1 |= FLAG_REG_RBP;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("bpl")) == 0) {
        registerDef1 |= FLAG_REG_RBP;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("rsp")) == 0) {
        registerDef1 |= FLAG_REG_RSP;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("esp")) == 0) {
        registerDef1 |= FLAG_REG_RSP;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("sp")) == 0) {
        registerDef1 |= FLAG_REG_RSP;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("spl")) == 0) {
        registerDef1 |= FLAG_REG_RSP;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r8")) == 0) {
        registerDef1 |= FLAG_REG_R8;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r8d")) == 0) {
        registerDef1 |= FLAG_REG_R8;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r8w")) == 0) {
        registerDef1 |= FLAG_REG_R8;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r8b")) == 0) {
        registerDef1 |= FLAG_REG_R8;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r9")) == 0) {
        registerDef1 |= FLAG_REG_R9;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r9d")) == 0) {
        registerDef1 |= FLAG_REG_R9;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r9w")) == 0) {
        registerDef1 |= FLAG_REG_R9;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r9b")) == 0) {
        registerDef1 |= FLAG_REG_R9;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r10")) == 0) {
        registerDef1 |= FLAG_REG_R10;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r10d")) == 0) {
        registerDef1 |= FLAG_REG_R10;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r10w")) == 0) {
        registerDef1 |= FLAG_REG_R10;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r10b")) == 0) {
        registerDef1 |= FLAG_REG_R10;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r11")) == 0) {
        registerDef1 |= FLAG_REG_R11;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r11d")) == 0) {
        registerDef1 |= FLAG_REG_R11;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r11w")) == 0) {
        registerDef1 |= FLAG_REG_R11;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r11b")) == 0) {
        registerDef1 |= FLAG_REG_R11;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r12")) == 0) {
        registerDef1 |= FLAG_REG_R12;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r12d")) == 0) {
        registerDef1 |= FLAG_REG_R12;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r12w")) == 0) {
        registerDef1 |= FLAG_REG_R12;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r12b")) == 0) {
        registerDef1 |= FLAG_REG_R12;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r13")) == 0) {
        registerDef1 |= FLAG_REG_R13;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r13d")) == 0) {
        registerDef1 |= FLAG_REG_R13;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r13w")) == 0) {
        registerDef1 |= FLAG_REG_R13;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r13b")) == 0) {
        registerDef1 |= FLAG_REG_R13;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r14")) == 0) {
        registerDef1 |= FLAG_REG_R14;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r14d")) == 0) {
        registerDef1 |= FLAG_REG_R14;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r14w")) == 0) {
        registerDef1 |= FLAG_REG_R14;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r14b")) == 0) {
        registerDef1 |= FLAG_REG_R14;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("r15")) == 0) {
        registerDef1 |= FLAG_REG_R15;
        registerDef2 |= FLAG_REG_R;
    } else if (registerName.compare(string("r15d")) == 0) {
        registerDef1 |= FLAG_REG_R15;
        registerDef2 |= FLAG_REG_E;
    } else if (registerName.compare(string("r15w")) == 0) {
        registerDef1 |= FLAG_REG_R15;
        registerDef2 |= FLAG_REG_X;
    } else if (registerName.compare(string("r15b")) == 0) {
        registerDef1 |= FLAG_REG_R15;
        registerDef2 |= FLAG_REG_L;
    } else if (registerName.compare(string("xmm0")) == 0) {
        registerDef1 |= FLAG_REG_XMM0;
    } else if (registerName.compare(string("xmm1")) == 0) {
        registerDef1 |= FLAG_REG_XMM1;
    } else if (registerName.compare(string("xmm2")) == 0) {
        registerDef1 |= FLAG_REG_XMM2;
    } else if (registerName.compare(string("xmm3")) == 0) {
        registerDef1 |= FLAG_REG_XMM3;
    } else if (registerName.compare(string("xmm4")) == 0) {
        registerDef1 |= FLAG_REG_XMM4;
    } else if (registerName.compare(string("xmm5")) == 0) {
        registerDef1 |= FLAG_REG_XMM5;
    } else if (registerName.compare(string("xmm6")) == 0) {
        registerDef1 |= FLAG_REG_XMM6;
    } else if (registerName.compare(string("xmm7")) == 0) {
        registerDef1 |= FLAG_REG_XMM7;
    } else if (registerName.compare(string("xmm8")) == 0) {
        registerDef1 |= FLAG_REG_XMM8;
    } else if (registerName.compare(string("xmm9")) == 0) {
        registerDef1 |= FLAG_REG_XMM9;
    } else if (registerName.compare(string("xmm10")) == 0) {
        registerDef1 |= FLAG_REG_XMM10;
    } else if (registerName.compare(string("xmm11")) == 0) {
        registerDef1 |= FLAG_REG_XMM11;
    } else if (registerName.compare(string("xmm12")) == 0) {
        registerDef1 |= FLAG_REG_XMM12;
    } else if (registerName.compare(string("xmm13")) == 0) {
        registerDef1 |= FLAG_REG_XMM13;
    } else if (registerName.compare(string("xmm14")) == 0) {
        registerDef1 |= FLAG_REG_XMM14;
    } else if (registerName.compare(string("xmm15")) == 0) {
        registerDef1 |= FLAG_REG_XMM15;
    } else if (registerName.compare(string("ymm0")) == 0) {
        registerDef1 |= FLAG_REG_YMM0;
    } else if (registerName.compare(string("ymm1")) == 0) {
        registerDef1 |= FLAG_REG_YMM1;
    } else if (registerName.compare(string("ymm2")) == 0) {
        registerDef1 |= FLAG_REG_YMM2;
    } else if (registerName.compare(string("ymm3")) == 0) {
        registerDef1 |= FLAG_REG_YMM3;
    } else if (registerName.compare(string("ymm4")) == 0) {
        registerDef1 |= FLAG_REG_YMM4;
    } else if (registerName.compare(string("ymm5")) == 0) {
        registerDef1 |= FLAG_REG_YMM5;
    } else if (registerName.compare(string("ymm6")) == 0) {
        registerDef1 |= FLAG_REG_YMM6;
    } else if (registerName.compare(string("ymm7")) == 0) {
        registerDef1 |= FLAG_REG_YMM7;
    } else if (registerName.compare(string("ymm8")) == 0) {
        registerDef1 |= FLAG_REG_YMM8;
    } else if (registerName.compare(string("ymm9")) == 0) {
        registerDef1 |= FLAG_REG_YMM9;
    } else if (registerName.compare(string("ymm10")) == 0) {
        registerDef1 |= FLAG_REG_YMM10;
    } else if (registerName.compare(string("ymm11")) == 0) {
        registerDef1 |= FLAG_REG_YMM11;
    } else if (registerName.compare(string("ymm12")) == 0) {
        registerDef1 |= FLAG_REG_YMM12;
    } else if (registerName.compare(string("ymm13")) == 0) {
        registerDef1 |= FLAG_REG_YMM13;
    } else if (registerName.compare(string("ymm14")) == 0) {
        registerDef1 |= FLAG_REG_YMM14;
    } else if (registerName.compare(string("ymm15")) == 0) {
        registerDef1 |= FLAG_REG_YMM15;
    }
}

string Tracer::_genRegFlagColumn(const UINT64 regFlags)
{
    string str;

    if (regFlags & 0x0001) str.append("_CF");
    if (regFlags & 0x0004) str.append("_PF");
    if (regFlags & 0x0010) str.append("_AF");
    if (regFlags & 0x0040) str.append("_ZF");
    if (regFlags & 0x0080) str.append("_SF");
    if (regFlags & 0x0100) str.append("_TF");
    if (regFlags & 0x0200) str.append("_IF");
    if (regFlags & 0x0400) str.append("_DF");
    if (regFlags & 0x0800) str.append("_OF");

    return str;
}

string Tracer::_genInsBin(const INS ins)
{
    ADDRINT pc = INS_Address(ins);

    unsigned char buf[32];
    memset(buf, 0, 32);

    PIN_SafeCopy(buf, (VOID*)pc, INS_Size(ins));

    stringstream ss;
    for (size_t i = 0; i < INS_Size(ins); i++) {
        ss << hex << setfill('0') << setw(2) << (buf[i] & 0xFF);
    }

    return ss.str();
}

VOID Tracer::Instrumentation(INS ins, VOID *v)
{
    assert(INS_Valid(ins));

    ADDRINT pc = INS_Address(ins);
    string disas = INS_Disassemble(ins);
    string imgName = _getImgName(pc);
    string rtnName = RTN_FindNameByAddress(pc);

    // cache disas
    _disasCache.setDisas(pc, disas);
    _disasCache.setInsBin(pc, string(gOpt.printInsBin ? _genInsBin(ins) : ""));

    // hook or skip
    if (gOpt.traceRules.insIsInSkipImg(imgName)) return;
    if (gOpt.traceRules.insIsInSkipRtn(rtnName)) return;
    if (gOpt.traceRules.insIsInSkipRange(pc))    return;

    // first ins
    if (_isFirstInstruction()) {
        INS_InsertCall(
            ins,
            IPOINT_BEFORE,
            (AFUNPTR)Tracer::_AnalysisInsFirst,
            IARG_INST_PTR,
            IARG_CONTEXT,
            IARG_END);
    }

    // ins
    INS_InsertCall(
        ins,
        IPOINT_BEFORE,
        (AFUNPTR)Tracer::_AnalysisIns,
        IARG_INST_PTR,
        IARG_CONTEXT,
        IARG_END);

    // ins branch
    if (INS_IsBranch(ins) || INS_IsCall(ins) || INS_IsRet(ins)) {
        INS_InsertCall(
            ins,
            IPOINT_TAKEN_BRANCH,
            (AFUNPTR)Tracer::_AnalysisInsBlanch,
            IARG_INST_PTR,
            IARG_CONTEXT,
            IARG_END);
    }

    // reg reads
    REG readReg = INS_RegR(ins, 0);
    
    for (UINT32 i = 0; REG_valid(readReg); ) {
        UINT64 registersRead1, registersRead2;
        _setupRegValue(registersRead1, registersRead2, REG_StringShort(readReg));

        if (registersRead1 != 0x0) {
            INS_InsertCall(
                ins,
                IPOINT_BEFORE,
                (AFUNPTR)Tracer::_AnalysisRegisterRead,
                IARG_INST_PTR,
                IARG_UINT64, registersRead1,
                IARG_UINT64, registersRead2,
                IARG_CONTEXT,
                IARG_END);
        }

        i++;
        readReg = INS_RegR(ins, i);
    }

    // reg writes
    REG writeReg = INS_RegW(ins, 0);

    for (UINT32 i = 0; REG_valid(writeReg); ) {
        UINT64 registersWrite1, registersWrite2;
        _setupRegValue(registersWrite1, registersWrite2, REG_StringShort(writeReg));

        if (registersWrite1 != 0x0) {
            if (disas.find("rep") != 0) { // to avoid assertion error (LEVEL_VM::AssertTargetIaddrValid: 606: assertion failed: tragetAddrValid)
                if (INS_HasFallThrough(ins)) {
                    INS_InsertCall(
                        ins,
                        IPOINT_AFTER,
                        (AFUNPTR)Tracer::_AnalysisRegisterWrite,
                        IARG_INST_PTR,
                        IARG_UINT64, registersWrite1,
                        IARG_UINT64, registersWrite2,
                        IARG_CONTEXT,
                        IARG_END);
                }
            }

            if (INS_IsBranch(ins) || INS_IsCall(ins)) {
                INS_InsertCall(
                    ins,
                    IPOINT_TAKEN_BRANCH,
                    (AFUNPTR)Tracer::_AnalysisRegisterWrite,
                    IARG_INST_PTR,
                    IARG_UINT64, registersWrite1,
                    IARG_UINT64, registersWrite2,
                    IARG_CONTEXT,
                    IARG_END);
            }
        }

        i++;
        writeReg = INS_RegW(ins, i);
    }

    // mem reads
    for (UINT32 i = 0; i < INS_MemoryOperandCount(ins); i++) {
        if (INS_MemoryOperandIsRead(ins, i)) {
            INS_InsertCall(
                ins,
                IPOINT_BEFORE,
                (AFUNPTR)Tracer::_AnalysisMemoryRead,
                IARG_INST_PTR,
                IARG_MEMORYREAD_EA,
                IARG_UINT64, INS_MemoryOperandSize(ins, i),
                IARG_CONTEXT,
                IARG_END);
        }
    }

    // mem writes
    if (INS_IsMemoryWrite(ins)) {
        INS_InsertCall(
            ins,
            IPOINT_BEFORE,
            (AFUNPTR)Tracer::_AnalysisMemoryWriteBefore,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);
        
        if (INS_HasFallThrough(ins)) {
            INS_InsertCall(
                ins,
                IPOINT_AFTER,
                (AFUNPTR)Tracer::_AnalysisMemoryWrite,
                IARG_INST_PTR,
                IARG_END);
        }

        if (INS_IsBranch(ins) || INS_IsCall(ins)) {
            INS_InsertCall(
                ins,
                IPOINT_TAKEN_BRANCH,
                (AFUNPTR)Tracer::_AnalysisMemoryWrite,
                IARG_INST_PTR,
                IARG_END);
        }
    }

    if (INS_IsValidForIpointAfter(ins)) {
        if (disas.find("rep") != 0) { // to avoid assertion error (ad-hoc)
            INS_InsertCall(
                ins,
                IPOINT_AFTER,
                (AFUNPTR)_AnalysisInsAfter,
                IARG_INST_PTR,
                IARG_CONTEXT,
                IARG_END);
        }
    }
}

VOID Tracer::_RewriteBeforeIns(ADDRINT pc, CONTEXT *ctx)
{
    //PIN_THREAD_UID tid = PIN_ThreadUid();
}

VOID Tracer::_RewriteAfterIns(ADDRINT pc, CONTEXT *ctx)
{
    //PIN_THREAD_UID tid = PIN_ThreadUid();

    // TODO: Rewrite returned value

    /*
    if (gOpt.anti_SetInformationThread) {
        if (_tiManager.getTiApiName(tid) == "ZwSetInformationThread") {
            gConsole.outInfo("Returned ZwSetInformationThread() without any operations\n");
            PIN_SetContextReg(ctx, REG_EIP, pc+5);
            PIN_ExecuteAt(ctx);
        }
    }

    if (gOpt.anti_isDebuggerPresent) {
        if (_tiManager.getTiApiName(tid) == "IsDebuggerPresent") {
            gConsole.outInfo("Modified a return of isDebuggerPresent()\n");
            PIN_SetContextReg(ctx, REG_RAX, 0x0);
            PIN_ExecuteAt(ctx);
        }
    }

    if (gOpt.anti_QueryInformationProcess) {
        if (_tiManager.getTiApiName(tid) == "ZwQueryInformationProcess") {
            gConsole.outInfo("Modified a return of ZwQueryInformationProcess()\n");
            PIN_SetContextReg(ctx, REG_RAX, 0x0);
            PIN_ExecuteAt(ctx);
        }
    }

    if (gOpt.anti_QueryObject) {
        if (_tiManager.getTiApiName(tid) == "ZwQueryObject") {
            ADDRINT *pObjectInformation;
            int zero = 0;

            pObjectInformation = (ADDRINT *)(PIN_GetContextReg(ctx, REG_RBP) - 0x04);
            if (*pObjectInformation != 0) {
                gConsole.outInfo("Modified NumberOfObjectsTypes of ZwQueryObject(): " + to_hex(*pObjectInformation) + " -> 0\n");
                PIN_SafeCopy(pObjectInformation, &zero, 4);
            }
        }
    }

    if (gOpt.anti_CheckRemoteDebuggerPresent) {
        if (_tiManager.getTiApiName(tid) == "CheckRemoteDebuggerPresent") {
            ADDRINT *ppbDebuggerPresent, *pbDebuggerPresent;
            int zero = 0;

            ppbDebuggerPresent = (ADDRINT *)(PIN_GetContextReg(ctx, REG_RSP) - 0x04);
            pbDebuggerPresent = (ADDRINT *)*ppbDebuggerPresent;

            if (*pbDebuggerPresent != 0) {
                gConsole.outInfo("Modified pbDebuggerPresent of CheckRemoteDebuggerPresent(): " + to_hex(*pbDebuggerPresent) + " -> 0\n");
                PIN_SafeCopy(pbDebuggerPresent, &zero, 4);
            }
        }
    }
    */
}

VOID Tracer::_AnalysisInsFirst(ADDRINT pc, CONTEXT *ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    _dispInitMessage();

    ADDRINT pFS, pPEB;
    UINT32 zero = 0, two = 2;

    pFS = PIN_GetContextReg(ctx, REG_SEG_FS_BASE);
    pPEB = pFS + 0x30;

    if (gOpt.anti_GlobalFlag) {
        ADDRINT *NtGlobalFlag;

        NtGlobalFlag = (ADDRINT *)(*(ADDRINT *)pPEB + 0x68);
        gConsole.outInfo("Modified PEB.GlobalFlag: " + to_hex(*NtGlobalFlag) + " -> 0\n");
        PIN_SafeCopy(NtGlobalFlag, &zero, 4);
    }

    if (gOpt.anti_HeapFlags) {
        ADDRINT* ProcessHeap, * PatchAddr;
        ProcessHeap = (ADDRINT*)(*(ADDRINT*)pPEB + 0x18);

        //PatchAddr = (ADDRINT *)(*ProcessHeap + 0x0C); // depends on OS 
        PatchAddr = (ADDRINT*)(*ProcessHeap + 0x40);
        if (*PatchAddr != 2) {
            gConsole.outInfo("Modified PEB.ProcessHeap+0x0C: " + to_hex(*PatchAddr) + " -> 2\n");
            PIN_SafeCopy(PatchAddr, &two, 4);
        }

        //PatchAddr = (ADDRINT *)(*ProcessHeap + 0x10); // depends on OS
        PatchAddr = (ADDRINT*)(*ProcessHeap + 0x44);
        if (*PatchAddr != 0) {
            gConsole.outInfo("Modified PEB.ProcessHeap+0x10: " + to_hex(*PatchAddr) + " -> 0\n");
            PIN_SafeCopy(PatchAddr, &zero, 4);
        }
    }
}

VOID Tracer::_AnalysisApiCall(ADDRINT rtnAddr,
                              ADDRINT retAddr,
                              CONTEXT *ctx)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    if (!_tiManager.getTiTracingEnabled(tid))
        return;
    
    if (gOpt.isFilteringInsideApi) {
        if (_tiManager.getTiRetAddr(tid)) {
            return;
        }
    }
    
    string imgName = _getImgName(rtnAddr);
    
    // 3 ways to get the rtn_name
    //string rtnName = _rtnCache.getRtn(rtnAddr);
    //string rtnName = RTN_FindNameByAddress(rtnAddr);
    string rtnName = _getRtnName(rtnAddr);

    string msgApiCall = string("#API call (trace ") + 
                        string(gOpt.isFilteringInsideApi ? "out" : "in") + string(") #") + 
                        imgName + " #" + rtnName + " " +
                        "#ret=" + to_hex(retAddr);

    //_tiManager.writeTiTracefile(tid, msgApiCall);
    gConsole.outInfo(msgApiCall + "\n", VERBOSE_INFO);

    string allRegs = " #" + _genRegGeneral(ctx);

    if (gOpt.printRegXmm) { allRegs += "_" + _genRegXmm(ctx); }
    if (gOpt.printRegYmm) { allRegs += "_" + _genRegYmm(ctx); }
    
    //_tiManager.writeTiTracefile(tid, allRegs);
    //_tiManager.writeTiTracefile(tid, "\n");

    if (gOpt.traceRules.inDetectApiList(rtnName)) {
        gConsole.outInfo("#API found: " + rtnName + "\n");
    }

    if (gOpt.modeCryptApiDetect) {
        if (_magic.isInCryptApiList(rtnName)) {
            gConsole.outInfo("#Crypt-API found: " + rtnName + "\n");
        }
    }

    if (gOpt.isFilteringInsideApi) {
        _tiManager.setTiRetAddr(tid, retAddr, rtnName);
    }
}

VOID Tracer::_AnalysisException()
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (!_tiManager.initThreadInfoMap(tid))
        return;

    _tiManager.writeTiTracefile(tid, "\n# API CALL: EXCEPTION!");
}

VOID Tracer::ImgInstrumentation(IMG img, VOID* val)
{
    gConsole.outInfo(_getImageInfo(img));

    string imgName = _getImgName(img);

    if (IMG_IsMainExecutable(img)) {
        _tiManager.setMainTid(PIN_ThreadUid());

        if (gOpt.isStartingFromEntryPoint) {
            gOpt.startAddress = IMG_EntryAddress(img);
        }

        if (gOpt.isStartingFromMainImage) {
            gOpt.startAddressRangeLow = IMG_LowAddress(img);
            gOpt.startAddressRangeHigh = IMG_LowAddress(img) + IMG_SizeMapped(img);
        }
    }

    gConsole.outInfo(_getSectionInfo(img), VERBOSE_INFO);
    gConsole.outInfo(_getRtnInfo(img), VERBOSE_DEBUG);

    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {

            ADDRINT rtnAddr = RTN_Address(rtn);
            string rtnName = RTN_Name(rtn);

            _rtnCache.setRtn(rtnAddr, rtnName);

            if (rtnName == "KiUserExceptionDispatcher") {
                if (gOpt.isFilteringInsideApi) {
                    RTN_Open(rtn);
                    RTN_InsertCall(
                        rtn,
                        IPOINT_BEFORE,
                        (AFUNPTR)_AnalysisException,
                        IARG_END);
                    RTN_Close(rtn);
                }

                continue;
            }

            RTN_Open(rtn);
            RTN_InsertCall(
                rtn,
                IPOINT_BEFORE,
                (AFUNPTR)_AnalysisApiCall,
                IARG_ADDRINT, rtnAddr,
                IARG_RETURN_IP,
                IARG_CONTEXT,
                IARG_END);
            RTN_Close(rtn);
        }
    }
}

VOID Tracer::_dispInitMessage()
{
    gConsole.outInfo("Tracing started ...\n");
    gConsole.outInfo(gOpt.getSettingsForDisplay());
    gConsole.outInfo(gOpt.getDebugInfoForDisplay(), VERBOSE_DEBUG);
}

VOID Tracer::_enableTracingIfStart(ADDRINT pc)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (gOpt.startAddress) {
        if (pc == gOpt.startAddress) {
            _tiManager.setTiTracingEnabled(tid, true);

            if (gOpt.isStartingFromEntryPoint) {
                gOpt.isFilteringInsideApi = gOpt.isFilteringInsideApiOrg;
            }
        }
    }

    if (gOpt.startAddressRangeLow) {
        if (gOpt.startAddressRangeLow <= pc && pc <= gOpt.startAddressRangeHigh) {
            _tiManager.setTiTracingEnabled(tid, true);

            if (gOpt.isStartingFromMainImage) {
                gOpt.isFilteringInsideApi = gOpt.isFilteringInsideApiOrg;
            }
        }
    }
}

VOID Tracer::_disableTracingIfEnd(ADDRINT pc)
{
    PIN_THREAD_UID tid = PIN_ThreadUid();

    if (gOpt.endAddress) {
        if (pc == gOpt.endAddress) {
            _tiManager.setTiTracingEnabled(tid, false);
        }
    }

    // count limit
    if (gOpt.traceExitInsLimit > 0) {
        UINT32 logCount = _tiManager.getTiLogCount(tid);

        if (logCount > gOpt.traceExitInsLimit) {
            gConsole.outInfo("Instruction count limit exceeded.\n");

            for (PIN_THREAD_UID tid : _tiManager.getTidList()) {
                _tiManager.writeTiLine(tid);
                _tiManager.clearTiLine(tid);
                _tiManager.closeTiTracefile(tid, "\nInstruction count limit exceeded.\n");
            }

            _tiManager.setTiTracingEnabled(tid, false);
        }
    }

    // time limit
    if (gOpt.traceExitTimeLimit > 0) {
        if (_getRunningTime() < gOpt.traceExitTimeLimit) {
            gConsole.outInfo("Time limit exceeded.\n");

            for (PIN_THREAD_UID tid : _tiManager.getTidList()) {
                _tiManager.writeTiLine(tid);
                _tiManager.clearTiLine(tid);
                _tiManager.closeTiTracefile(tid, "\nTime limit exceeded.\n");
            }

            _tiManager.setTiTracingEnabled(tid, false);
        }
    }
}

VOID Tracer::AfterForkInChild(THREADID threadid, const CONTEXT* ctx, VOID* arg)
{
#ifdef __GNUC__
    UINT32 pid = getpid();
#endif
#ifdef _WIN64
    UINT32 pid = WINDOWS::GetCurrentProcessId();
#endif

    _tiManager.setPid(pid);

    for (PIN_THREAD_UID tid : _tiManager.getTidList()) {
        _tiManager.refreshTiTracefile(tid);
    }

    gConsole.outInfo("Child process is created: " + std::to_string(pid) + "\n");
}

VOID Tracer::Finalize(INT32 code, VOID *v)
{
    for (PIN_THREAD_UID tid : _tiManager.getTidList()) {
        _tiManager.writeTiLine(tid);
        _tiManager.clearTiLine(tid);
        _tiManager.closeTiTracefile(tid, "# End of trace\n");
        _tiManager.clearTi(tid);
    }

    ostringstream ss;
    ss << "Tracing finished (";
    ss << fixed << setprecision(3) << dec << ((float)(_getRunningTime())/1000) << " Sec)\n\n";
    ss << resetiosflags(ios_base::floatfield);

    gConsole.outInfo(ss.str());
    gConsole.close();
}

VOID Tracer::_dumpMemory(const ADDRINT pc, const struct MemAddrSize memInfo, CONTEXT* ctx)
{
    ADDRINT memAddr = memInfo.addr;
    size_t memSize = memInfo.size;

    if (!memAddr) {
        memAddr = PIN_GetContextReg(ctx, REG_RSP);
    }

    // TODO: avoid access violation

    string imgName = _getImgName(memAddr);
    string apiName = _getRtnName(pc);
    // TODO: section name

    gConsole.out(imgName + ":" + apiName + ":" + to_hex(pc) + "\n");

    string str;

    for (size_t i = 0; i < memSize; i += 16) {
        string view1, view2;
        view1 += to_hex(memAddr + i) + "  ";

        for (size_t j = 0; j < 16 && i + j < memSize - 1; j++) {
            UINT32 chr;
            
            chr = *(UINT32*)(memAddr + i + j) & 0xFF;
            view1 += to_hex(chr, 2) + " ";
            view2 += (0x20 <= chr && chr <= 0x7e) ? (CHAR)chr : '.';

            if (j == 7) {
                view1 += " ";
                view2 += " ";
            }
        }

        view1.resize(62, ' ');
        str += view1 + "  " + view2 + "\n";
    }

    gConsole.out(str + "\n\n");
}

VOID Tracer::_dumpReg(const ADDRINT pc, const string allRegs)
{
    gConsole.out(to_hex(pc) + ":" + allRegs + "\n");
}

string Tracer::_getOsVersion()
{
    char osVer[128];
    OS_GetOSVersion(osVer, 128);

    return string(osVer);
}

string Tracer::_getImgName(const ADDRINT pc)
{
    PIN_LockClient();

    IMG img = IMG_FindByAddress(pc);

    PIN_UnlockClient();

    return _getImgName(img);
}

string Tracer::_getImgName(const IMG img)
{
    string imgName;

    if (IMG_Valid(img)) {
        imgName = IMG_Name(img);

#ifdef __GNUC__
        size_t pos = imgName.find_last_of("/");
#endif

#ifdef _WIN64
        size_t pos = imgName.find_last_of("\\");
#endif

        if (pos != string::npos) {
            imgName = imgName.substr(pos + 1);
        }
    }

    return imgName;
}

string Tracer::_getRtnName(const ADDRINT rtnAddr)
{
    string apiName;

    apiName = RTN_FindNameByAddress(rtnAddr);

    if (apiName == "unnamedImageEntryPoint") {
        return "";
    }

    return apiName;
}

string Tracer::_getImageInfo(const IMG img)
{
    TableForOutput table("Image info");

    table.pushRow("Name:", IMG_Name(img));
    table.pushRow("Address:", to_hex(IMG_LowAddress(img)) + "-" + to_hex(IMG_HighAddress(img)) + " (" + to_hex(IMG_SizeMapped(img)) + ")");
    table.pushRow("Entry:", to_hex(IMG_EntryAddress(img)));

    if (gOpt.verboseLevel >= VERBOSE_INFO) {
        table.pushRow("Type:", to_hex(IMG_Type(img)));
        table.pushRow("Gp:", to_hex(IMG_Gp(img)));
        table.pushRow("LoadOffset:", to_hex(IMG_LoadOffset(img)));
    }

    return table.genTableString();
}

string Tracer::_getSectionInfo(const IMG img)
{
    TableForOutput table("Section info");

    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
        string col1, col2, col3, col4;

        col1 = SEC_Name(sec);
        col2 = "(" + to_hex(SEC_Address(sec)) + "-" + to_hex(SEC_Address(sec) + SEC_Size(sec)) + ") ";
        col3 = string(SEC_IsReadable(sec)   ? "r" : "-") +
               string(SEC_IsWriteable(sec)  ? "w" : "-") +
               string(SEC_IsExecutable(sec) ? "x" : "-");
        col4 = (SEC_Mapped(sec) ? "mapped" : " ");

        table.pushRow(col1, col2, col3, col4);
    }

    return table.genTableString();
}

string Tracer::_getRtnInfo(const IMG img)
{
    TableForOutput table("Routine info");

    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
            string col1, col2, col3;

            col1 = to_hex(RTN_Address(rtn)) + "-" + to_hex(RTN_Address(rtn) + RTN_Range(rtn));
            col2 = " (" + to_hex(RTN_Range(rtn)) + "): ";
            col3 = RTN_Name(rtn); // API name

            table.pushRow(col1, col2, col3);
        }
    }

    return table.genTableString();
}

