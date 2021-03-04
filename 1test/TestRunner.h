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

#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include "TestInterface.h"
#include "TestPlugin.h"
#include "TraceOutput.h"

namespace pet {

template<class> class LinkedPtrList;

class TestRunner {
	template<class> friend class TestBase;
	template<class> friend class LinkedPtrList;

	static TestInterface* currentTest;
	static TestOutput* output;
	static bool isSynthetic;
	static bool failing;
	static LinkedPtrList<TestPlugin*> plugins;

	struct Result {
		int synthetic = 0, failed = 0;
	};

	static Result runTest(TestInterface*);

public:
	static bool installPlugin(TestPlugin*);

	static int runAllTests(TestOutput* output = &TraceOutput::instance);
	static inline TestInterface* getCurrentTest();

	static inline bool isFailing() { return failing; }
    static void failTest(const char* sourceInfo, const char* text);
    static void failTestAlways(const char* sourceInfo, const char* text);
    static void checkExpectation(bool condition, const char* sourceInfo, const char* text);
    static void checkExpectationAlways(bool condition, const char* sourceInfo, const char* text);

	class Experimental;
};

////////////////////////////////////////////////////////////////

inline TestInterface* TestRunner::getCurrentTest() {
    return currentTest;
}

}

#endif /* TESTRUNNER_H_ */
