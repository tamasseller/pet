/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#include "1test/TestRunner.h"
#include "1test/TestOutput.h"

#include <cstring>
#include <iostream>
#include <set>

using namespace pet;

struct FailureRecord {
	std::string testName;
	std::string sourceInfo;
	std::string failureSourceInfo;

	inline FailureRecord (const char* testName, const char* sourceInfo, const char *failureSourceInfo):
		testName(testName) {
		this->sourceInfo = rindex(sourceInfo, '/') ? rindex(sourceInfo, '/') + 1: sourceInfo;
		this->failureSourceInfo = rindex(failureSourceInfo, '/') ? rindex(failureSourceInfo, '/') + 1: failureSourceInfo;
	}

	bool operator <(const FailureRecord& other) const {
		return testName + sourceInfo + failureSourceInfo < other.testName + other.sourceInfo + other.failureSourceInfo;
	}

	bool operator ==(const FailureRecord& other) const {
		return testName + sourceInfo + failureSourceInfo == other.testName + other.sourceInfo + other.failureSourceInfo;
	}

};

////
////
////

bool printActualFailureRecords = false;

int totalExpected = 25;

std::set<FailureRecord> expectedFailures{
	FailureRecord("FunctionNotOk", "MetaTest2.cpp:31", "MetaTest2.cpp:24"),
	FailureRecord("CheckFalse", "MetaTest1.cpp:33", "MetaTest1.cpp:34"),
	FailureRecord("Failure", "MetaTest1.cpp:25", "MetaTest1.cpp:26")
};

////
////
////


struct MetaTestOutput: public TestOutput {
	uint32_t progress = 0;
	uint32_t total = 0;
	uint32_t failed = 0;

	std::set<FailureRecord> failures;

	virtual void reportProgress() {
		progress++;
	}

	virtual void reportTestFailure(const char* testName, const char* sourceInfo, const char *failureSourceInfo) {
		if(printActualFailureRecords)
			std::cout << "\"" << testName << "\", \"" << sourceInfo << "\", \"" <<  failureSourceInfo  << "\"" << std::endl;
		failures.insert(FailureRecord(testName, sourceInfo, failureSourceInfo));
	}


	virtual void reportFinalFailure(uint32_t total, uint32_t failure) {
		this->total = total;
		this->failed = failure;
	}

	virtual void reportFinalSuccess(uint32_t total) {
		this->total = total;
		this->failed = 0;
	}

	virtual inline ~MetaTestOutput() {}
};

int main(int ac, char** av)
{
	MetaTestOutput output;

	int ret = 0;

    if(pet::TestRunner::runAllTests(&output) != -1) {
    	std::cerr << "runAllTests return value error" << std::endl;
    	ret = -1;
    } else if(totalExpected != output.total){
    	std::cerr << "number of tests error (expected " << totalExpected << " got " << output.total << ")" << std::endl;
    	ret = -1;
    } else if(totalExpected != output.progress){
    	std::cerr << "progress reporting error (expected " << totalExpected << " got " << output.progress << ")" << std::endl;
    	ret = -1;
    } else if(expectedFailures.size() != output.failed){
    	std::cerr << "final number of failures reporting error (expected " << expectedFailures.size() << " got " << output.failed << ")" << std::endl;
    	ret = -1;
    } else if(expectedFailures.size() != output.failures.size()) {
    	std::cerr << "number of failures error (expected " << expectedFailures.size() << " got " << output.failures.size() << ")" << std::endl;
    	ret = -1;
    } else {
    	bool bad = false;
    	for(auto &x: expectedFailures) {
    		auto y = output.failures.find(x);
    		if(y == output.failures.end()) {
    			std::cerr << "missing failure (" << x.testName << ")" << std::endl;
    			ret = -1;
    		}
    	}
    }

    return ret;
}
