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
	std::string text;

	inline FailureRecord (const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text):
		testName(testName), text(text) {
		this->sourceInfo = rindex(sourceInfo, '/') ? rindex(sourceInfo, '/') + 1: sourceInfo;
		this->failureSourceInfo = rindex(failureSourceInfo, '/') ? rindex(failureSourceInfo, '/') + 1: failureSourceInfo;
	}

	bool operator <(const FailureRecord& other) const {
		return sourceInfo + testName + failureSourceInfo < other.sourceInfo + other.testName + other.failureSourceInfo;
	}
};

////
////
////

bool printActualFailureRecords = false;

int totalExpected = 33;

std::set<FailureRecord> expectedFailures{
	FailureRecord("NonEmptyMessageFailure", "TestNoGroup.cpp:37", "TestNoGroup.cpp:38", "NonEmpty"),
	FailureRecord("EmptyMessageFailure", "TestNoGroup.cpp:33", "TestNoGroup.cpp:34", ""),
	FailureRecord("CheckFalse", "TestNoGroup.cpp:29", "TestNoGroup.cpp:30", "Expectation: 'false' failed"),
	FailureRecord("DisabledIndependentUnexpected@Mock", "TestMock.cpp:149", "--- Finalization ---", "Mock - unmet expectations"),
	FailureRecord("ParamsPointerWrong@Mock", "TestMock.cpp:132", "TestMock.cpp:135", "Mock - pointer parameter error"),
	FailureRecord("ParamsDataWrongLength@Mock", "TestMock.cpp:118", "TestMock.cpp:122", "Mock - data block parameter error"),
	FailureRecord("ParamsDataWrong@Mock", "TestMock.cpp:110", "TestMock.cpp:114", "Mock - data block parameter error"),
	FailureRecord("ParamsStringWrong@Mock", "TestMock.cpp:89", "TestMock.cpp:93", "Mock - string parameter error"),
	FailureRecord("ParamsWrong@Mock", "TestMock.cpp:77", "TestMock.cpp:79", "Mock - parameter error"),
	FailureRecord("ParamsNoParamConsumed@Mock", "TestMock.cpp:67", "--- Finalization ---", "Mock - unmet expectations"),
	FailureRecord("ParamsNoParamExpected@Mock", "TestMock.cpp:62", "TestMock.cpp:64", "Mock - parameter error"),
	FailureRecord("IndependentSourcesFail@Mock", "TestMock.cpp:52", "TestMock.cpp:54", "Mock - unexpected call"),
	FailureRecord("Unexpected@Mock", "TestMock.cpp:40", "TestMock.cpp:41", "Mock - unexpected call"),
	FailureRecord("WrongOrder@Mock", "TestMock.cpp:34", "TestMock.cpp:37", "Mock - unexpected call"),
	FailureRecord("MultipleIncomplete@Mock", "TestMock.cpp:28", "--- Finalization ---", "Mock - unmet expectations"),
	FailureRecord("Incomplete@Mock", "TestMock.cpp:17", "--- Finalization ---", "Mock - unmet expectations"),
	FailureRecord("SetupNotOk@OtherGroup", "TestGroups.cpp:60", "TestGroups.cpp:56", "User message"),
	FailureRecord("BodyNotOk@Group", "TestGroups.cpp:47", "TestGroups.cpp:28", "Expectation: 'ok' failed"),
	FailureRecord("TeardownNotOk@Group", "TestGroups.cpp:44", "TestGroups.cpp:28", "Expectation: 'ok' failed")
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

	virtual void reportTestFailure(const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char* text) {
	    FailureRecord record(testName, sourceInfo, failureSourceInfo, text);

        if(printActualFailureRecords)
            std::cout << "\"" << record.testName << "\", \"" << record.sourceInfo << "\", \"" <<  record.failureSourceInfo << "\", \"" << record.text << "\"" << std::endl;

        failures.insert(record);
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

    	if(!ret)
    	    std::cerr << "OK (expected and got " << output.failures.size() << " failures out of " << totalExpected << " tests with correct reports)" << std::endl;
    }

    return ret;
}
