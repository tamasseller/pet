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
}

void TraceOutput::reportTestFailure(const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text)
{
	trace::warn << "\n";
    trace::warn << "Test '" << testName << "' (" << sourceInfo <<  ")\n";
    trace::warn << "\n";

    trace::warn << "    failed at " << failureSourceInfo;

    if(text)
        trace::warn << ": " << text;

    trace::warn << "\n";
    trace::warn << "\n";
}

void TraceOutput::reportFinalFailure(uint32_t total, uint32_t failure)
{
    trace::warn << "\n";
	trace::info << "ERROR (" << failure << " of " << total << " tests failed)\n";
}

void TraceOutput::reportFinalSuccess(uint32_t total)
{
    trace::warn << "\n";
	trace::info << "OK (all " << total << " tests have been ran successfully)\n";
}


