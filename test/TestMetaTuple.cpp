/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

TEST(Tuple, Sanity) {
    pet::Tuple<int, char> simpleOne = pet::Tuple<int, char>(1, '1');
    pet::Tuple<int, char> simpleTwo = pet::Tuple<int, char>(2, '2');

    CHECK(simpleOne.get<0>() == 1);
    CHECK(simpleOne.get<1>() == '1');
    CHECK(simpleTwo.get<0>() == 2);
    CHECK(simpleTwo.get<1>() == '2');
}

TEST(Tuple, Factory) {
    auto factorized = makeTuple(0xf00, "bar");

    CHECK(factorized .get<0>() == 0xf00);
    CHECK(factorized .get<1>() == "bar");
}

TEST(Tuple, Extract) {
    auto vector = makeTuple(3, 1, 4, 1, 5);

    long long output[5];
    vector.extract(output);

    CHECK(output[0] == 3);
    CHECK(output[1] == 1);
    CHECK(output[2] == 4);
    CHECK(output[3] == 1);
    CHECK(output[4] == 5);
}

TEST(Tuple, EqualsOperator) {
    auto src = makeTuple(1, 'a');
    auto dst = makeTuple(2, 'b');
    dst = src;

    CHECK(dst.get<0>() == 1);
    CHECK(dst.get<1>() == 'a');
}

TEST(Tuple, EqualsOperatorReference) {
    auto src = makeTuple(1, 'a');
    int a = 2;
    char c = 'b';
    auto dst = pet::Tuple<int&, char&>(a, c);
    dst = src;

    CHECK(a == 1);
    CHECK(c == 'a');
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
