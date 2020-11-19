/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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

#include "1test/Test.h"
#include "1test/Mock.h"

#include "ubiquitous/Failable.h"

using namespace pet;

/*
 * Test usage
 */
struct ExpResult: public Failable<ExpResult, int> {
	inline constexpr ExpResult(const int &value): Failable<ExpResult, int>(value) {}
	inline constexpr ExpResult(): Failable<ExpResult, int>() {}

	enum ErrorValues {
		error = -1
	};

	inline bool isError() {
		return value == error;
	}
};

ExpResult testExp(int x)
{
	if(x < 0)
		return ExpResult::error;

	int ret = 1;
	while(x--)
		ret *= 2;

	return ret;
}

ExpResult testExpTimesTwo(int x)
{
	ExpResult result = testExp(x);
	if(result.failed())
		return ExpResult::error;

	return result*2;
}

ExpResult testWrongExpTimesTwo(int x)
{
	return testExp(x) * 2;
}


TEST_GROUP(Failable)
{
	TEST_SETUP() {
		assertMocked = true;
	}
	TEST_TEARDOWN() {
		assertMocked = false;
	}
};

TEST(Failable, TotallyCareless)
{
    MOCK()::EXPECT(uncheckedErrorReport);

	testExp(2);
}

TEST(Failable, NoErrorHasCheck)
{
	ExpResult result = testExp(2);

	bool failure = result.failed();
	CHECK(!failure);

	int value = result;
	CHECK(value == 4);
}

TEST(Failable, ErroneousHasCheck)
{
	ExpResult result = testExp(-1);

	bool failure = result.failed();
	CHECK(failure);
}

TEST(Failable, PorpagateHappy)
{
    ExpResult result = testExpTimesTwo(1);

	bool failure = result.failed();
	CHECK(!failure);

    int value = result;
    CHECK(value == 4);
}

TEST(Failable, AssignHappy)
{
    ExpResult result;
    result = testExpTimesTwo(1);

    bool failure = result.failed();
    CHECK(!failure);

    int value = result;
    CHECK(value == 4);
}

TEST(Failable, ReAssignNoCheck)
{
    MOCK()::EXPECT(uncheckedErrorReport);

    ExpResult result = testExpTimesTwo(1);

    // No failure check

    result = testExpTimesTwo(1);
    CHECK(!result.failed());
}

TEST(Failable, ReAssignResultCorrect)
{
    ExpResult result = testExpTimesTwo(1);
    CHECK(!result.failed());
    CHECK(result == 4);
    result = testExpTimesTwo(2);
    CHECK(!result.failed());
    CHECK(result == 8);
}

TEST(Failable, NoErrorNoCheck)
{
    MOCK()::EXPECT(uncheckedErrorReport);

	ExpResult result = testExp(2);
	int value = result;
	CHECK(value == 4);
}

TEST(Failable, ErroneousNoCheck)
{
    MOCK()::EXPECT(uncheckedErrorReport);

	ExpResult result = testExp(-1);
	int value = result;
}

TEST(Failable, PorpagateWrongMiddleUser)
{
    MOCK()::EXPECT(uncheckedErrorReport);

    ExpResult result = testWrongExpTimesTwo(1);

	bool failure = result.failed();
	CHECK(!failure);

    int value = result;
    CHECK(value == 4);
}

TEST(Failable, PorpagateWrongEndUserHappy)
{
    MOCK()::EXPECT(uncheckedErrorReport);

    ExpResult result = testExpTimesTwo(1);

    int value = result;
    CHECK(value == 4);
}

TEST(Failable, PorpagateWrongEndUserErroneous)
{
    MOCK()::EXPECT(uncheckedErrorReport);

    ExpResult result = testExpTimesTwo(-1);
    volatile int value = result;
    value = value;
}


