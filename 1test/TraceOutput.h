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
	virtual void reportProgress();
	virtual void reportTestFailure(const char* testName, const char* sourceInfo, const char *failureSourceInfo);
	virtual void reportFinalFailure(uint32_t total, uint32_t failure);
	virtual void reportFinalSuccess(uint32_t total) ;
	virtual inline ~TraceOutput() {}

public:
	static TraceOutput instance;
};

}


#endif /* TRACEOUTPUT_H_ */
