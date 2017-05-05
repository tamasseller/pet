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
