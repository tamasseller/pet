/*
 * Test.h
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#ifndef TEST_H_
#define TEST_H_

#include "TestHandle.h"

template<class Child>
class TestBase: public TestHandle {
protected:
	TestBase();
	static Child instance;
};

////////////////////////////////////////////////////////////////

#include "TestRunner.h"

template<class Child>
Child TestBase<Child>::instance;

template<class Child>
TestBase<Child>::TestBase()
{
	TestRunner::registerTest(&instance);
}

#endif /* TEST_H_ */
