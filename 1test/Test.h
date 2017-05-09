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

#ifndef MACROS_H_
#define MACROS_H_

#include "TestBase.h"
#include "TestRunner.h"
#include "Mock.h"

#define TEST_SETUP() inline void testSetup()
#define TEST_TEARDOWN() inline void testTeardown()

namespace pet {
class TestGroupBase {
    protected:
        TEST_SETUP() {}
        TEST_TEARDOWN() {}
};
}

#define TEST_CLASS_NAME_HELPER(name, group)     Test ## name ## In ## group ## Handle
#define INTERNAL_TEST_CLASS_NAME(name, group)   TEST_CLASS_NAME_HELPER(name, group)

#define TEST_GROUP_NAME_HELPER(name)            Test ## name ## Group
#define INTERNAL_TEST_GROUP_NAME(name)          TEST_GROUP_NAME_HELPER(name)

#define INTERNAL_STRINGIFY_2(x) 		        #x
#define INTERNAL_STRINGIFY(x)			        INTERNAL_STRINGIFY_2(x)
#define INTERNAL_AT()					        __FILE__ ":" INTERNAL_STRINGIFY(__LINE__)

#define INTERNAL_TEST(name, group, parent, gName)    							        \
																						\
class INTERNAL_TEST_CLASS_NAME(name, group):                                            \
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
        instance.dummy();                                                               \
	    testSetup();                                                                    \
		testBody();																		\
		testTeardown();                                                                 \
	}																					\
};																						\
																						\
void INTERNAL_TEST_CLASS_NAME(name, group)::testBody() 								    \

#define TEST_GROUP(name)                                                                \
struct INTERNAL_TEST_GROUP_NAME(name): protected pet::TestGroupBase

TEST_GROUP(_dummy_) {};

#define TEST_WITH_GROUP(group, test) INTERNAL_TEST(test, group, group, "@" INTERNAL_STRINGIFY(group))
#define TEST_WITHOUT_GROUP(test) INTERNAL_TEST(test, /* none */, _dummy_, "")
#define TEST_SWITCH_HELPER(dummy1, dummy2, real, ...) real
#define TEST(...) TEST_SWITCH_HELPER(__VA_ARGS__, TEST_WITH_GROUP, TEST_WITHOUT_GROUP)(__VA_ARGS__)

#define FAIL(...)						pet::TestRunner::failTest(INTERNAL_AT())
#define CHECK(x) 						{if(!(x)) FAIL(); }


#endif /* MACROS_H_ */
