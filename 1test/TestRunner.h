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

#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include "data/LinkedList.h"

#include "TestHandle.h"
#include "TraceOutput.h"

namespace pet {

class TestRunner {
	template<class>
	friend class TestBase;

	static inline void registerTest(TestHandle* test);

	static pet::LinkedList<TestHandle> tests;

	static TestHandle* currentTest;
	static TestOutput* output;
public:
	static int runAllTests(TestOutput* output = &TraceOutput::instance);
	static int failTest(const char* sourceInfo);
};

////////////////////////////////////////////////////////////////

inline void TestRunner::registerTest(TestHandle* test) {
	tests.add(test);
}

}

#endif /* TESTRUNNER_H_ */
