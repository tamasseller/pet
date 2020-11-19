/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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

#include "algorithm/CharLookupTable.h"

using namespace pet;

TEST_GROUP(CharLookupTable) {};

TEST(CharLookupTable, OddEvent)
{
	CHECK(CharLookupTable<0, 2>::contains(0));
	CHECK(!CharLookupTable<0, 2>::contains(1));
	CHECK(CharLookupTable<0, 2>::contains(2));
	CHECK(!CharLookupTable<0, 2>::contains(3));

	CHECK(!CharLookupTable<1, 3>::contains(0));
	CHECK(CharLookupTable<1, 3>::contains(1));
	CHECK(!CharLookupTable<1, 3>::contains(2));
	CHECK(CharLookupTable<1, 3>::contains(3));
}

TEST(CharLookupTable, FarLimits)
{
	CHECK(!CharLookupTable<1, 100>::contains(0));
	CHECK(CharLookupTable<1, 100>::contains(1));
	CHECK(!CharLookupTable<1, 100>::contains(2));

	CHECK(!CharLookupTable<1, 100>::contains(99));
	CHECK(CharLookupTable<1, 100>::contains(100));
	CHECK(!CharLookupTable<1, 100>::contains(101));

	CHECK(!CharLookupTable<1, 100>::contains(200));
}
