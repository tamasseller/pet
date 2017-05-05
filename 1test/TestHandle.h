/*
 * TestFwd.h
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#ifndef TESTFWD_H_
#define TESTFWD_H_

class TestRunner;

namespace pet {
template<class> class LinkedList;
}

class TestHandle
{
	friend TestRunner;
	virtual void runTest() = 0;
	virtual const char* getName() = 0;
	virtual const char* getSourceInfo() = 0;

	friend pet::LinkedList<TestHandle>;
	TestHandle* next;
protected:
	inline void dummy() {}

public:
	inline virtual ~TestHandle() {}
};

#endif /* TESTFWD_H_ */
