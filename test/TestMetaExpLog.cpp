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

#include "meta/ExpLog.h"

using namespace pet;

TEST_GROUP(Exp) {};

TEST(Exp, TwoAdZero) {
	auto val = exp<2>::x<0>::value;
	CHECK(val == 1);
}

TEST(Exp, ThreeAdZero) {
	auto val = exp<3>::x<0>::value;
	CHECK(val == 1);
}

TEST(Exp, TwoAdOne) {
	auto val = exp<2>::x<1>::value;
	CHECK(val == 2);
}

TEST(Exp, FiveAdOne) {
	auto val = exp<5>::x<1>::value;
	CHECK(val == 5);
}

TEST(Exp, TwoAdTen) {
	auto val = exp<2>::x<10>::value;
	CHECK(val == 1024);
}

TEST(Exp, SevenAdFive) {
	auto val = exp<7>::x<5>::value;
	CHECK(val == 16807);
}

TEST(Exp, ElevenAdThree) {
	auto val = exp<11>::x<3>::value;
	CHECK(val == 1331);
}

TEST_GROUP(Log) {};

TEST(Log, Log2Two) {
	auto val = log<2>::x<2>::value;
	CHECK(val == 1);
}

TEST(Log, Log3Nine) {
	auto val = log<3>::x<9>::value;
	CHECK(val == 2);
}

TEST(Log, Log3Seven) {
	auto val = log<3>::x<7>::value;
	CHECK(val == 1);
}

TEST(Log, Log2Fourteen) {
	auto val = log<2>::x<14>::value;
	CHECK(val == 3);
}
