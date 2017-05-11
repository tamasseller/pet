/*
 * TraceOutput.cpp
 *
 *  Created on: 2017.05.09.
 *      Author: tooma
 */

#include "TraceOutput.h"
#include "ubiquitous/Trace.h"

using namespace pet;

typedef Trace<TestTraceTag> trace;

TraceOutput TraceOutput::instance;

void TraceOutput::reportProgress()
{
	trace::info << ".";

    if(nDots == 64) {
        trace::info << "\n";
        nDots = 0;
    }else
        nDots++;
}

void TraceOutput::reportTestFailure(bool isSynthetic, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text)
{
	trace::warn << "\n";
    trace::warn << (isSynthetic ? "Synthetic test based on '" : "Test '") << testName << "' (" << sourceInfo <<  ")\n";
    trace::warn << "\n";

    trace::warn << "    failed at " << failureSourceInfo;

    if(text)
        trace::warn << ": " << text;

    trace::warn << "\n";
    trace::warn << "\n";
}

void TraceOutput::reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic)
{
    trace::warn << "\n";

    if(failure) {
        trace::info << "ERROR (" << failure << " of " << normal << " regular ";

        if(synthetic)
            trace::info << "plus " << synthetic << " synthetic ";

        trace::info << "tests failed)\n";
    } else {
        trace::info << "OK (all " << normal << " regular ";

        if(synthetic)
            trace::info << "and " << synthetic << " synthetic ";

        trace::info << "tests have been ran successfully)\n";
    }

    trace::warn << "\n";
}
