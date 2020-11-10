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

#include "meta/Range.h"
#include "meta/ExpLog.h"

using namespace pet;

TEST_GROUP(ExpRange) {};

TEST(ExpRange, BaseTwoOneToTen) {
	for(int i=0; i<10; i++) {
		auto val = applyOverRange<exp<2>::x, 1, 10>::value[i];
		CHECK(val == 2 << i);
	}
}

TEST(ExpRange, BaseThreeZeroToFifteen) {
	unsigned int x = 1;
	for(int i=0; i<15; i++) {
		auto val = applyOverRange<exp<3>::x, 0, 15>::value[i];
		CHECK(val == x);
		x *= 3;
	}
}
