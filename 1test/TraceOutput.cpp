/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

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
    trace::info << "\n";

    if(failure) {
        trace::info << "ERROR (" << failure << " of " << normal << " regular ";

        if(synthetic)
            trace::info << "plus " << synthetic << " synthetic ";

        trace::info << "tests failed)\n";
    } else {
    	if(normal) {
			trace::info << "OK (all " << normal << " regular ";

			if(synthetic)
				trace::info << "and " << synthetic << " synthetic ";

			trace::info << "tests have been ran successfully)\n";
    	} else {
    		trace::info << "No tests registered to run !\n";
    	}
    }

    trace::info << "\n";
}
