/*
 * TestRunner.cpp
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

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
