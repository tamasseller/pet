/*
 * TestRunner.h
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include "TestHandle.h"
#include "data/LinkedList.h"

class TestTraceTag;

class TestRunner {
	template<class>
	friend class TestBase;

	static inline void registerTest(TestHandle* test);

	static pet::LinkedList<TestHandle> tests;

public:
	static int runAllTests();
	static int failTest(const char* sourceInfo);
};

////////////////////////////////////////////////////////////////

inline void TestRunner::registerTest(TestHandle* test) {
	tests.add(test);
}

#endif /* TESTRUNNER_H_ */
