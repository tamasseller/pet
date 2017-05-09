/*
 * Output.h
 *
 *  Created on: 2017.05.09.
 *      Author: tooma
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <stdint.h>

namespace pet {

class TestOutput {
	friend class TestRunner;
	virtual void reportProgress() = 0;
	virtual void reportTestFailure(const char* testName, const char* sourceInfo, const char *failureSourceInfo) = 0;
	virtual void reportFinalFailure(uint32_t total, uint32_t failure) = 0;
	virtual void reportFinalSuccess(uint32_t total) = 0;
protected:
	virtual inline ~TestOutput() {}
};

}

#endif /* OUTPUT_H_ */
