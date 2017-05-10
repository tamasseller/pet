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

#include "data/DoubleList.h"

using namespace pet;

namespace {
	class SimpleElement {
	public:
		SimpleElement *next, *prev;
		SimpleElement(): next(0), prev(0) {}
	};

	SimpleElement e1, e2, e3;
}

TEST_GROUP(EmptyDoubleList) {
	DoubleList<SimpleElement> list;
};

TEST(EmptyDoubleList, EmptyAdd) {
	CHECK(list.add(&e1));
}

TEST(EmptyDoubleList, RepeatedAdd) {
	CHECK(list.add(&e1));
	CHECK(!list.add(&e1));
}

TEST(EmptyDoubleList, EmptyAddBack) {
	CHECK(list.addBack(&e1));
}

TEST(EmptyDoubleList, RepeatedAddBack) {
	CHECK(list.addBack(&e1));
	CHECK(!list.addBack(&e1));
}

TEST(EmptyDoubleList, EmptyRemove) {
	CHECK(!list.remove(&e1));
}

TEST(EmptyDoubleList, ValidRemove) {
	CHECK(list.add(&e1));
	CHECK(list.remove(&e1));
}

TEST(EmptyDoubleList, RepeatedRemove) {
	CHECK(list.add(&e1));
	CHECK(list.remove(&e1));
	CHECK(!list.remove(&e1));
}

TEST(EmptyDoubleList, OverIterate) {
	auto it = list.iterator();
	CHECK(it.current() == 0);
	it.step();
	CHECK(it.current() == 0);
	it.step();
	CHECK(it.current() == 0);
}

TEST_GROUP(NonEmptyDoubleList) {
	DoubleList<SimpleElement> list;

	TEST_SETUP() {
		CHECK(list.addBack(&e1));
		CHECK(list.add(&e2));
		CHECK(list.add(&e3));
	}
};

TEST(NonEmptyDoubleList, RemoveNonFirst) {
	CHECK(list.remove(&e2));

	CHECK(!list.add(&e1));
	CHECK(list.add(&e2));
	CHECK(!list.add(&e3));
}

TEST(NonEmptyDoubleList, RemoveNonFirstAddBack) {
	CHECK(list.remove(&e2));

	CHECK(!list.add(&e1));
	CHECK(list.addBack(&e2));
	CHECK(!list.add(&e3));
}


TEST(NonEmptyDoubleList, Iterate) {
	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyDoubleList, OverIterate) {
	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
	it.step();
	CHECK(it.current() == 0);
	it.step();
	CHECK(it.current() == 0);
}


TEST(NonEmptyDoubleList, IterateAfterLastRemoved) {
	CHECK(list.remove(&e1));

	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyDoubleList, IterateAfterMiddleRemoved) {
	CHECK(list.remove(&e2));

	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyDoubleList, IterateAfterFirstRemoved) {
	CHECK(list.remove(&e3));

	auto it = list.iterator();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyDoubleList, IterateAfterReAdd) {
	CHECK(list.remove(&e2));
	CHECK(list.add(&e2));

	auto it = list.iterator();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyDoubleList, IterateAfterReAddBack) {
	CHECK(list.remove(&e2));
	CHECK(list.addBack(&e2));

	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == 0);
}
