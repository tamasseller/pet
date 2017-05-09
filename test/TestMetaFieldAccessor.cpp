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

#include "meta/FieldAccessor.h"

using namespace pet;

struct C {
	int x = 12, y = 23;
};

struct B {
	int z = 34;
	C c;
};

struct A {
	B b2;
	int w = 45;
	B b1;
};


TEST_GROUP(MemberPointers) {};

TEST(MemberPointers, DirectFields) {
	A a;
	int wA = access(a, &A::w);
	CHECK(wA == 45);

	B b;
	int zB = access(b, &B::z);
	CHECK(zB == 34);

	C c;
	int yC = access(c, &C::y);
	CHECK(yC == 23);

	int xC = access(c, &C::x);
	CHECK(xC == 12);
}

TEST(MemberPointers, TwoLevelSimple) {
	B b;
	int xCB = access(b, &B::c, &C::x);
	CHECK(xCB == 12);
}

TEST(MemberPointers, TwoLevelSimpleNonZeroOffset) {
	B b;
	int yCB = access(b, &B::c, &C::y);
	CHECK(yCB == 23);
}

TEST(MemberPointers, TwoLevelMultipleOnFirstLevel) {
	A a;
	int zB1A = access(a, &A::b1, &B::z);
	CHECK(zB1A == 34);
}

TEST(MemberPointers, TwoLevelMultipleOnFirstLevelNonZeroOffset) {
	A a;
	int zB2A = access(a, &A::b1, &B::z);
	CHECK(zB2A == 34);
}
