/*
 * Test1test.cpp
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#include "CppUTest/TestHarness.h"

#include "1test/TestRunner.h"

#include "ubiquitous/Trace.h"

#include <sstream>
#include <iostream>

TEST_GROUP(RealUnitTest) {
};

TEST(RealUnitTest, RunAllTests) {
	writerRecords = true;
	CHECK(TestRunner::runAllTests() != 0);
	writerRecords = false;

	std::istringstream stream(recorded);
	std::string line;
	recorded = "";

	int nErrors = 0;

	bool errorFound = false;

	while (std::getline(stream, line)) {
		if(line.compare(0, 4, "Test") == 0) {
			nErrors++;
		} else if(line.compare(0, 5, "ERROR") == 0) {
			CHECK(!errorFound);
			errorFound = true;
		}
	}

	CHECK(errorFound);
	CHECK(nErrors == 3);
}
