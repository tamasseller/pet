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

#include "1test/Test.h"

#include "meta/TypePackInfo.h"

using namespace pet;

TEST_GROUP(TypesInfo) {};

TEST(TypesInfo, Position) {
    CHECK(TypePackInfo<short, int, char>::Position<short>::value == 0);
    CHECK(TypePackInfo<short, int, char>::Position<int>::value == 1);
    CHECK(TypePackInfo<short, int, char>::Position<char>::value == 2);
}

TEST(TypesInfo, Size) {
    CHECK(TypePackInfo<>::size == 0);
    CHECK(TypePackInfo<char>::size == sizeof(char));
    CHECK(TypePackInfo<char, short>::size == sizeof(short));
    CHECK(TypePackInfo<int, char, short>::size == sizeof(int));
    CHECK(TypePackInfo<int, char, long long, short>::size == sizeof(long long));
}

TEST(TypesInfo, Alignment) {
    CHECK(TypePackInfo<>::alignment == 0);
    CHECK(TypePackInfo<char>::alignment == alignof(char));
    CHECK(TypePackInfo<char, short>::alignment == alignof(short));
    CHECK(TypePackInfo<int, char, short>::alignment == alignof(int));
    CHECK(TypePackInfo<int, char, long long, short>::alignment == alignof(long long));
}

TEST(TypesInfo, Uniqueness) {
    CHECK(!TypePackInfo<int, char, short, int>::allUnique);
    CHECK(!TypePackInfo<int, char, short, int>::allUnique);
    CHECK(!TypePackInfo<short, char, short, int>::allUnique);
    CHECK(!TypePackInfo<char, char, short, int>::allUnique);
    CHECK(TypePackInfo<float, char, short, int>::allUnique);
    CHECK(!TypePackInfo<int, int, int>::allUnique);
    CHECK(!TypePackInfo<int, char, int>::allUnique);
    CHECK(!TypePackInfo<char, int, char>::allUnique);
    CHECK(!TypePackInfo<char, short, char>::allUnique);
    CHECK(TypePackInfo<char, short, int>::allUnique);
    CHECK(TypePackInfo<short, int, char>::allUnique);
}

TEST(TypesInfo, NoReferences) {
    CHECK(TypePackInfo<short, int, char>::noReferences);
    CHECK(!TypePackInfo<short, int, char&>::noReferences);
    CHECK(!TypePackInfo<short&, int&, char&>::noReferences);

    typedef int &S;
    CHECK(!TypePackInfo<S>::noReferences);
}


