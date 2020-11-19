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

#include "meta/Tuple.h"

using namespace pet;

TEST_GROUP(Tuple) {};

TEST(Tuple, Sanity)
{
    pet::Tuple<int, char> simpleOne = pet::makeTuple(1, '1');
    pet::Tuple<int, char> simpleTwo = pet::makeTuple(2, '2');

    CHECK(simpleOne.get<0>() == 1);
    CHECK(simpleOne.get<1>() == '1');
    CHECK(simpleTwo.get<0>() == 2);
    CHECK(simpleTwo.get<1>() == '2');
}

TEST(Tuple, Extract)
{
    auto vector = makeTuple(3, 1, 4, 1, 5);

    long long output[5];
    vector.extract(output);

    CHECK(output[0] == 3);
    CHECK(output[1] == 1);
    CHECK(output[2] == 4);
    CHECK(output[3] == 1);
    CHECK(output[4] == 5);
}

TEST(Tuple, EqualsOperator)
{
    auto src = makeTuple(1, 'a');
    auto dst = makeTuple(2, 'b');
    dst = src;

    CHECK(dst.get<0>() == 1);
    CHECK(dst.get<1>() == 'a');
}

TEST(Tuple, EqualsOperatorReference)
{
    auto src = makeTuple(1, 'a');
    int a = 2;
    char c = 'b';
    auto dst = pet::makeTuple<int&, char&>(a, c);
    dst = src;

    CHECK(a == 1);
    CHECK(c == 'a');
}

TEST(Tuple, SimpleApply)
{
    struct S
    {
       static inline void f(int x, char c)
       {
           MOCK(TupleApplyTarget)::CALL(f).withParam(x).withParam(c);
       }
    };

    MOCK(TupleApplyTarget)::EXPECT(f).withParam(1).withParam('a');
    makeTuple(1, 'a').moveApply(&S::f);

    MOCK(TupleApplyTarget)::EXPECT(f).withParam(1).withParam('a');
    makeTuple(1, 'a').copyApply(&S::f);

    auto t = makeTuple(1, 'a');
    MOCK(TupleApplyTarget)::EXPECT(f).withParam(1).withParam('a');
    pet::move(t).moveApply(&S::f);

    MOCK(TupleApplyTarget)::EXPECT(f).withParam(1).withParam('a');
    t.copyApply(&S::f);
}

TEST(Tuple, MoveOnlyApply)
{
    struct S
    {
        S() = default;
        S(S&&) = default;
        S(const S&) = delete;

        static inline void f(S) {
            MOCK(TupleApplyTarget)::CALL(f);
        }

        static inline void g(S&&) {
            MOCK(TupleApplyTarget)::CALL(g);
        }
    };

    MOCK(TupleApplyTarget)::EXPECT(f);
    makeTuple(S()).moveApply(&S::f);

    MOCK(TupleApplyTarget)::EXPECT(g);
    makeTuple(S()).moveApply(&S::g);
}

#if 0
// GCC 8 says: "sorry, unimplemented: mangling reference_type"
TEST(Tuple, Tie) {
    int a = 2;
    char c = 'b';
    tie(a, c) = makeTuple(1, 'a');

    CHECK(a == 1);
    CHECK(c == 'a');
}
#endif
