/*
 * Macros.h
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#ifndef MACROS_H_
#define MACROS_H_

#include "TestBase.h"
#include "TestRunner.h"

#define TEST_CLASS_NAME_HELPER(name) 	Test ## name ## Handle
#define INTERNAL_TEST_CLASS_NAME(name) 	TEST_CLASS_NAME_HELPER(name)

#define INTERNAL_STRINGIFY_2(x) 		#x
#define INTERNAL_STRINGIFY(x)			INTERNAL_STRINGIFY_2(x)
#define INTERNAL_AT()					__FILE__ ":" INTERNAL_STRINGIFY(__LINE__)


#define TEST(name)																		\
																						\
class INTERNAL_TEST_CLASS_NAME(name): public TestBase<INTERNAL_TEST_CLASS_NAME(name)> {	\
	virtual const char* getName() {														\
		return INTERNAL_STRINGIFY(name);												\
	}																					\
																						\
	virtual const char* getSourceInfo() {												\
		return INTERNAL_AT();								\
	}																					\
																						\
	void testBody();																	\
																						\
	virtual void runTest()																\
	{																					\
		instance.dummy();																\
		testBody();																		\
	}																					\
};																						\
																						\
void INTERNAL_TEST_CLASS_NAME(name)::testBody() 										\

#define FAIL() 							TestRunner::failTest(INTERNAL_AT())
#define CHECK(x) 						{if(!(x)) FAIL(); }


#endif /* MACROS_H_ */
