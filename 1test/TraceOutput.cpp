/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

void TraceOutput::reportProgress(const char* name, bool synth) {
	trace::info() << "Completed test #" << ++testCounter << ": " << (synth ? "synthetic test based on " : "regular test ") << name;
}

void TraceOutput::reportTestFailure(uint32_t rerunIdx, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text)
{
	if(rerunIdx)
	{
		trace::warn() << pet::endl << "Synthetic test #" << rerunIdx << " based on '" << testName << "' (" << sourceInfo <<  ")" << pet::endl << pet::endl
			<< "    failed at " << failureSourceInfo << ((text) ? ": " : "") << ((text) ? text : "") << pet::endl;
	}
	else
	{
		trace::warn() << pet::endl << "Test '" << testName << "' (" << sourceInfo <<  ")" << pet::endl << pet::endl
			<< "    failed at " << failureSourceInfo << ((text) ? ": " : "") << ((text) ? text : "") << pet::endl;
	}
}

void TraceOutput::reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic)
{
    if(failure)
    {
        if(synthetic)
        {
        	trace::info() << pet::endl << "ERROR: " << failure << " of " << normal << " regular and " << synthetic << " synthetic tests failed !" << pet::endl;
        }
        else
        {
        	trace::info() << pet::endl << "ERROR: " << failure << " of " << normal << " tests failed !" << pet::endl;
        }
    }
    else
    {
    	if(normal)
    	{
			if(synthetic)
			{
				trace::info() << pet::endl << "OK: all " << normal << " regular and " << synthetic << " synthetic tests have been ran successfully." << pet::endl;
			}
			else
			{
				trace::info() << pet::endl << "OK: all " << normal << " tests have been ran successfully." << pet::endl;
			}
    	}
    	else
    	{
    		trace::info() << pet::endl << "No tests registered to run !" << pet::endl;
    	}
    }
}
