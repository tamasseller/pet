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

#include "TestRunner.h"

#include "ubiquitous/Trace.h"

#include <setjmp.h>

pet::LinkedList<TestHandle> TestRunner::tests;

typedef pet::Trace<TestTraceTag> trace;

static jmp_buf jmpBuff;
static TestHandle* currentTest;

int TestRunner::runAllTests()
{
	unsigned int run = 0, failed = 0;

	for(auto it = tests.iterator(); it.current(); it.step()) {
		trace::info << ".";
		currentTest = it.current();

		if(setjmp(jmpBuff))
			failed++;
		else
			currentTest->runTest();

		run++;
	}

	currentTest = 0;

	if(failed) {
		trace::info << "\nERROR (" << failed << " of " << run << " tests failed)\n";
		return -1;
	} else {
		trace::info << "\nOK (all " << run << "tests have been ran successfully)\n";
		return 0;
	}
}

int TestRunner::failTest(const char* srcInfo)
{
	trace::warn 	<< "\nTest '" << currentTest->getName()
					<< "' (" << currentTest->getSourceInfo() <<  ")\n\n\tfailed at " << srcInfo << "\n\n";

	longjmp(jmpBuff,1);
}
