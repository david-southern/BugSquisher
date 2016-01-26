#pragma once

#include "s3eDebug.h"

#define null NULL

#define SUBPIXEL_SHIFT 3

#define SUBPIXEL_TO_SCREEN(x) ((x) >> SUBPIXEL_SHIFT)
#define SCREEN_TO_SUBPIXEL(x) ((x) << SUBPIXEL_SHIFT)

class DebugTracer
{
    const char *function;
public:
    DebugTracer(const char *function)
    {
        this->function = function;
        s3eDebugTracePrintf("Entering function %s", function);
//        s3eDebugTraceFlush();
    }

    ~DebugTracer()
    {
        s3eDebugTracePrintf("Exiting function %s", function);
//        s3eDebugTraceFlush();
    }
};

// #define DO_DEBUG_TRACING 1

#ifdef DO_DEBUG_TRACING
#define DEBUG_TRACER(func) DebugTracer __tracer(#func);
#else
#define DEBUG_TRACER(func) ;
#endif