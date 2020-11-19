/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#if defined (__GNUC__) && !defined(__ECLIPSE__)
#define TEST(...)                       VAR_ARG_MACRO(TEST, ##__VA_ARGS__)
#define BEGIN_TEST_CASE(...)            VAR_ARG_MACRO(BEGIN_SPLIT, ##__VA_ARGS__)
#define IGNORE_TEST(...)                VAR_ARG_MACRO(IGNORE_TEST, ##__VA_ARGS__)
#else
#define TEST(group, name)               TEST2(group, name)
#define BEGIN_TEST_CASE(group, name)    BEGIN_SPLIT2(group, name)
#define IGNORE_TEST(group, name)        IGNORE_TEST2(group, name)
#endif

#define FAIL(text)                      pet::TestRunner::failTest(INTERNAL_AT(), text)
#define FAIL_ALWAYS(text)               pet::TestRunner::failTestAlways(INTERNAL_AT(), text)

#define CHECK_FAIL_TEXT(...)            ("Expectation: '" INTERNAL_STRINGIFY(__VA_ARGS__) "' failed")
#define CHECK(...)                      pet::TestRunner::checkExpectation((__VA_ARGS__), INTERNAL_AT(), CHECK_FAIL_TEXT(__VA_ARGS__))
#define CHECK_ALWAYS(...)               pet::TestRunner::checkExpectationAlways((__VA_ARGS__), INTERNAL_AT(), CHECK_FAIL_TEXT(__VA_ARGS__))

#define TEST_SETUP()                    inline void testSetup()
#define TEST_TEARDOWN()                 inline void testTeardown()

#define TEST_GROUP(name)                                                                \
struct INTERNAL_TEST_GROUP_NAME(name): public pet::TestGroupBase

////////////////////////////////////////////////////////////////////////////////////////////////

#define TEST1(test)                             INTERNAL_TEST(test, /* none */, _dummy_, "")
#define TEST2(group, test)                      INTERNAL_TEST(test, group, group, "@" INTERNAL_STRINGIFY(group))

#define BEGIN_SPLIT1(test)                      INTERNAL_BEGIN_SPLIT(test, /* none */, _dummy_, "")
#define BEGIN_SPLIT2(group, test)               INTERNAL_BEGIN_SPLIT(test, group, group, "@" INTERNAL_STRINGIFY(group))

#define IGNORE_TEST1(test)                      INTERNAL_IGNORE_TEST(test, /* none */, _dummy_)
#define IGNORE_TEST2(group, test)               INTERNAL_IGNORE_TEST(test, group, group)

#define TEST_CLASS_NAME_HELPER(name, group)     Test ## name ## In ## group ## Handle
#define INTERNAL_TEST_CLASS_NAME(name, group)   TEST_CLASS_NAME_HELPER(name, group)

#define TEST_WRAPPER_NAME_HELPER(name, group)   Test ## name ## In ## group ## Wrapper
#define INTERNAL_TEST_WRAPPER_NAME(name, group) TEST_WRAPPER_NAME_HELPER(name, group)

#define TEST_GROUP_NAME_HELPER(name)            Test ## name ## Group
#define INTERNAL_TEST_GROUP_NAME(name)          TEST_GROUP_NAME_HELPER(name)

TEST_GROUP(_dummy_) {};


/***************************************************************************************/
/*                                                                                     */
/*                   Initial part of class declaration for test case.                  */
/*                                                                                     */
/***************************************************************************************/

#define INTERNAL_TEST_CLASS_HEADER(name, group, parent)                                 \
struct INTERNAL_TEST_CLASS_NAME(name, group):                                           \
    public pet::TestBase<INTERNAL_TEST_CLASS_NAME(name, group)>,                        \
    public INTERNAL_TEST_GROUP_NAME(parent)                                             \


/***************************************************************************************/
/*                                                                                     */
/*                       Common members of test case classes.                          */
/*                                                                                     */
/***************************************************************************************/

#define INTERNAL_TEST_CLASS_MEMBERS(name, group, parent, gName)                         \
    typedef INTERNAL_TEST_GROUP_NAME(parent) Group;                                     \
    virtual const char* getName() {                                                     \
        return INTERNAL_STRINGIFY(name) gName;                                          \
    }                                                                                   \
                                                                                        \
    virtual const char* getSourceInfo() {                                               \
        return INTERNAL_AT();                                                           \
    }                                                                                   \
                                                                                        \
    virtual void runTest()                                                              \
    {                                                                                   \
        using Base = pet::TestBase<INTERNAL_TEST_CLASS_NAME(name, group)>;              \
        Base::instance.Base::dummy();                                                   \
        Base::run();                                                                    \
    }                                                                                   \


/***************************************************************************************/
/*                                                                                     */
/*      Compilation of all the bits that makes up test case class declaration          */
/*           and function declaration for the body of the test case                    */
/*                                                                                     */
/***************************************************************************************/

#define INTERNAL_TEST(name, group, parent, gName)                                       \
                                                                                        \
INTERNAL_TEST_CLASS_HEADER(name, group, parent) {                                       \
    INTERNAL_TEST_CLASS_MEMBERS(name, group, parent, gName)                             \
    void testBody();                                                                    \
};                                                                                      \
                                                                                        \
void INTERNAL_TEST_CLASS_NAME(name, group)::testBody()                                  \


/***************************************************************************************/
/*                                                                                     */
/*    Test case + test body macros are a split up version of the test macro, that      */
/*    allows it to be part of a template class declaration in order to facilitate      */
/*       parametric interface test suites implemented as such a template class.        */
/*                                                                                     */
/***************************************************************************************/

#define INTERNAL_BEGIN_SPLIT(name, group, parent, gName)                                \
INTERNAL_TEST_CLASS_HEADER(name, group, parent) {                                       \
    INTERNAL_TEST_CLASS_MEMBERS(name, group, parent, gName)                             \
    void testBody()                                                                     \

#define END_TEST_CASE() };

/***************************************************************************************/
/*                                                                                     */
/*                 Dummy declaration used for ignored test cases.                      */
/*                                                                                     */
/***************************************************************************************/

#define INTERNAL_IGNORE_TEST(name, group, parent)										\
																						\
struct INTERNAL_TEST_CLASS_NAME(name, group):                                           \
    public INTERNAL_TEST_GROUP_NAME(parent) {           	                            \
	void testBody();																	\
};																						\
																						\
void INTERNAL_TEST_CLASS_NAME(name, group)::testBody() 								    \


#endif /* TEST_H_ */
