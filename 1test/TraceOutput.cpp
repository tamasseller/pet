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

void TraceOutput::reportTestFailure(const char* testName, const char* sourceInfo, const char *failureSourceInfo)
{
	trace::warn 	<< "\nTest '" << testName << "' (" << sourceInfo <<  ")\n\n\tfailed at " << failureSourceInfo << "\n\n";
}

void TraceOutput::reportFinalFailure(uint32_t total, uint32_t failure)
{
	trace::info << "\nERROR (" << failure << " of " << total << " tests failed)\n";
}

void TraceOutput::reportFinalSuccess(uint32_t total)
{
	trace::info << "\nOK (all " << total << " tests have been ran successfully)\n";
}


