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

#ifndef TEST_H_
#define TEST_H_

#include "TestBase.h"
#include "TestRunner.h"
#include "Mock.h"

#include "Macro.h"

#define TEST(...)           VAR_ARG_MACRO(TEST, ##__VA_ARGS__)

#define FAIL(text)          pet::TestRunner::failTest(INTERNAL_AT(), text)

#define CHECK(x)            { if(!(x)) FAIL("Expectation: '" INTERNAL_STRINGIFY(x) "' failed"); }

#define TEST_SETUP()        inline void testSetup()
#define TEST_TEARDOWN()     inline void testTeardown()

#define TEST_GROUP(name)                                                                \
struct INTERNAL_TEST_GROUP_NAME(name): public pet::TestGroupBase

////////////////////////////////////////////////////////////////////////////////////////////////

#define TEST1(test)         INTERNAL_TEST(test, /* none */, _dummy_, "")
#define TEST2(group, test)  INTERNAL_TEST(test, group, group, "@" INTERNAL_STRINGIFY(group))

#define TEST_CLASS_NAME_HELPER(name, group)     Test ## name ## In ## group ## Handle
#define INTERNAL_TEST_CLASS_NAME(name, group)   TEST_CLASS_NAME_HELPER(name, group)

#define TEST_GROUP_NAME_HELPER(name)            Test ## name ## Group
#define INTERNAL_TEST_GROUP_NAME(name)          TEST_GROUP_NAME_HELPER(name)

TEST_GROUP(_dummy_) {};

#define INTERNAL_TEST(name, group, parent, gName)    							        \
																						\
struct INTERNAL_TEST_CLASS_NAME(name, group):                                           \
    public pet::TestBase<INTERNAL_TEST_CLASS_NAME(name, group)>,                        \
    public INTERNAL_TEST_GROUP_NAME(parent) {           	                            \
	virtual const char* getName() {														\
		return INTERNAL_STRINGIFY(name) gName;       	    							\
	}																					\
																						\
	virtual const char* getSourceInfo() {												\
		return INTERNAL_AT();								                            \
	}																					\
																						\
	void testBody();																	\
																						\
	virtual void runTest()																\
	{																					\
	    TestBase::instance.dummy();                                                     \
        run();                                                                          \
	}																					\
};																						\
																						\
void INTERNAL_TEST_CLASS_NAME(name, group)::testBody() 								    \

#endif /* TEST_H_ */
