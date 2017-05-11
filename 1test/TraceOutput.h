/*
 * TraceOutput.h
 *
 *  Created on: 2017.05.09.
 *      Author: tooma
 */

#ifndef TRACEOUTPUT_H_
#define TRACEOUTPUT_H_

#include "TestOutput.h"

class TestTraceTag;

namespace pet {

class TraceOutput: public TestOutput {
    int nDots = 0;

	virtual void reportProgress();
	virtual void reportTestFailure(bool isSynthetic, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text);
	virtual void reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic);
	virtual inline ~TraceOutput() {}

public:
	static TraceOutput instance;
};

}


#endif /* TRACEOUTPUT_H_ */
