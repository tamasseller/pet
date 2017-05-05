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
