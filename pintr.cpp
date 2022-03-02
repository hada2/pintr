#include "pintr.hpp"
#include "Options.hpp"
#include "Tracer.hpp"
#include "Utils.hpp"

// globals
PIN_LOCK gLockWrite;
Options gOpt;
Console gConsole;

int main(int argc, char **argv)
{
    if (PIN_Init(argc, argv)) {
        cout << gOpt.getHelpForDisplay() << endl;
        return 1;
    }

    PIN_InitSymbols();

    gOpt.getOptions(argc, argv);
    gConsole.open((gOpt.tracefileBaseName + "-console.log"));

    Tracer tracer;
    
    PIN_AddFiniFunction(tracer.Finalize, 0);
    IMG_AddInstrumentFunction(tracer.ImgInstrumentation, 0);
    INS_AddInstrumentFunction(tracer.Instrumentation, 0);

#ifdef __GNUC__
    PIN_AddForkFunction(FPOINT_AFTER_IN_CHILD, tracer.AfterForkInChild, 0);
#endif

    PIN_StartProgram();

    return 0;
}


