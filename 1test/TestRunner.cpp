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
#include "Mock.h"

#include <setjmp.h>

namespace pet {

LinkedList<TestHandle> TestRunner::tests;

static jmp_buf jmpBuff;

TestHandle* TestRunner::currentTest;
TestOutput* TestRunner::output;

int TestRunner::runAllTests(TestOutput* output)
{
	TestRunner::output = output;
	unsigned int run = 0, failed = 0;

	for(auto it = tests.iterator(); it.current(); it.step()) {
		output->reportProgress();
		currentTest = it.current();

		if(setjmp(jmpBuff))
			failed++;
		else
			currentTest->runTest();

		run++;
	}

	currentTest = 0;
	TestRunner::output = 0;

	if(failed) {
		output->reportFinalFailure(run, failed);
		return -1;
	} else {
		output->reportFinalSuccess(run);
		return 0;
	}
}

void TestRunner::failTest(const char* srcInfo, const char* text)
{
	output->reportTestFailure(currentTest->getName(), currentTest->getSourceInfo(), srcInfo, text);
	longjmp(jmpBuff,1);
}

}
