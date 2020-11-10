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

#include "meta/Packs.h"

using namespace pet;

TEST_GROUP(LastOf) {};

TEST(LastOf, Single) {
	CHECK(sizeof(lastOf<int>) == sizeof(int));
}

TEST(LastOf, Multiple) {
	CHECK(sizeof(lastOf<int, short, char>) == sizeof(char));
}

TEST(LastOf, MultipleSame) {
	CHECK(sizeof(lastOf<int, short, int>) == sizeof(int));
}

TEST_GROUP(FirstOf) {};

TEST(FirstOf, Single) {
	CHECK(sizeof(firstOf<int>) == sizeof(int));
}

TEST(FirstOf, Multiple) {
	CHECK(sizeof(firstOf<char, short, int>) == sizeof(char));
}

TEST(FirstOf, MultipleSame) {
	CHECK(sizeof(firstOf<int, short, int>) == sizeof(int));
}
