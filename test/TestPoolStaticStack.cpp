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

#include "pool/StaticStack.h"

#include <vector>
#include <iostream>

using namespace pet;

TEST_GROUP(StaticStack) {
	typedef StaticStack<int, 4> Uut;
	Uut uut;
};

TEST(StaticStack, Sanity) {
	int chk = 123;
	CHECK(uut.push(456));
	CHECK(uut.pop(chk));
	CHECK(chk == 456);
	CHECK(!uut.pop(chk));
}

TEST(StaticStack, Ordering) {
	int chk;
	CHECK(uut.push(1));
	CHECK(uut.push(2));
	CHECK(uut.push(3));
	CHECK(uut.pop(chk));
	CHECK(chk == 3);
	CHECK(uut.pop(chk));
	CHECK(chk == 2);
	CHECK(uut.pop(chk));
	CHECK(chk == 1);
	CHECK(!uut.pop(chk));
}

TEST(StaticStack, OverFill) {
	int chk;
	CHECK(uut.push(1));
	CHECK(uut.push(2));
	CHECK(uut.push(3));
	CHECK(uut.push(4));
	CHECK(!uut.push(5));
	CHECK(uut.pop(chk));
	CHECK(chk == 4);
	CHECK(uut.pop(chk));
	CHECK(chk == 3);
	CHECK(uut.pop(chk));
	CHECK(chk == 2);
	CHECK(uut.pop(chk));
	CHECK(chk == 1);
	CHECK(!uut.pop(chk));
}

TEST(StaticStack, Iterator) {
	CHECK(uut.push(1));
	CHECK(uut.push(2));
	CHECK(uut.push(3));

	Uut::Iterator it = uut.iterator();
	CHECK(it.current() && *it.current() == 1);
	it.step(uut);
	CHECK(it.current() && *it.current() == 2);
	it.step(uut);
	CHECK(it.current() && *it.current() == 3);
	it.step(uut);
	CHECK(!it.current());
	it.step(uut);
	CHECK(!it.current());
}

TEST(StaticStack, IteratorOnEmpty) {
	Uut::Iterator it = uut.iterator();
	CHECK(!it.current());
	it.step(uut);
	CHECK(!it.current());
}

TEST(StaticStack, Clear) {
	CHECK(uut.push(1));
	CHECK(uut.push(2));
	uut.clear();
	int dummy;
	CHECK(!uut.pop(dummy));
	CHECK(uut.push(3));
	CHECK(uut.push(4));
	CHECK(uut.push(5));
	CHECK(uut.push(6));
}
