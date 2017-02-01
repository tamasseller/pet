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

#include "CppUTest/TestHarness.h"

#include "LinkedList.h"

using namespace container;

class SimpleElement {
public:
	SimpleElement* next;
	SimpleElement(): next(0) {}
};

SimpleElement e1, e2, e3;

TEST_GROUP(EmptyLinkedList) {
	LinkedList<SimpleElement> list;
};

TEST(EmptyLinkedList, EmptyAdd) {
	CHECK(list.add(&e1));
}

TEST(EmptyLinkedList, RepeatedAdd) {
	CHECK(list.add(&e1));
	CHECK(!list.add(&e1));
}

TEST(EmptyLinkedList, EmptyAddBack) {
	CHECK(list.addBack(&e1));
}

TEST(EmptyLinkedList, RepeatedAddBack) {
	CHECK(list.addBack(&e1));
	CHECK(!list.addBack(&e1));
}

TEST(EmptyLinkedList, EmptyRemove) {
	CHECK(!list.remove(&e1));
}

TEST(EmptyLinkedList, ValidRemove) {
	CHECK(list.add(&e1));
	CHECK(list.remove(&e1));
}

TEST(EmptyLinkedList, RepeatedRemove) {
	CHECK(list.add(&e1));
	CHECK(list.remove(&e1));
	CHECK(!list.remove(&e1));
}

TEST(EmptyLinkedList, OverIterate) {
	auto it = list.iterator();
	CHECK(it.current() == 0);
	it.step();
	CHECK(it.current() == 0);
	it.step();
	CHECK(it.current() == 0);
}

TEST_GROUP(NonEmptyLinkedList) {
	LinkedList<SimpleElement> list;

	TEST_SETUP() {
		CHECK(list.addBack(&e1));
		CHECK(list.add(&e2));
		CHECK(list.add(&e3));
	}
};

TEST(NonEmptyLinkedList, RemoveNonFirst) {
	CHECK(list.remove(&e2));

	CHECK(!list.add(&e1));
	CHECK(list.add(&e2));
	CHECK(!list.add(&e3));
}

TEST(NonEmptyLinkedList, RemoveNonFirstAddBack) {
	CHECK(list.remove(&e2));

	CHECK(!list.add(&e1));
	CHECK(list.addBack(&e2));
	CHECK(!list.add(&e3));
}


TEST(NonEmptyLinkedList, Iterate) {
	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, OverIterate) {
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


TEST(NonEmptyLinkedList, IterateAfterLastRemoved) {
	CHECK(list.remove(&e1));

	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IterateAfterMiddleRemoved) {
	CHECK(list.remove(&e2));

	auto it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IterateAfterFirstRemoved) {
	CHECK(list.remove(&e3));

	auto it = list.iterator();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IterateAfterReAdd) {
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

TEST(NonEmptyLinkedList, IterateAfterReAddBack) {
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


TEST(NonEmptyLinkedList, IteratorRemoveFirst) {
	auto it = list.iterator();

	CHECK(it.current() == &e3);
	it.remove();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);

	it = list.iterator();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorRemoveSecond) {
	auto it = list.iterator();

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.remove();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);

	it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorRemoveLast) {
	auto it = list.iterator();

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.remove();
	CHECK(it.current() == 0);

	it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorRemoveEnd) {
	auto it = list.iterator();

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
	it.remove();

	it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorInsertFirst) {
	auto it = list.iterator();
	SimpleElement e4;

	CHECK(it.current() == &e3);
	it.insert(&e4);
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);

	it = list.iterator();
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorInsertSecond) {
	auto it = list.iterator();
	SimpleElement e4;

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.insert(&e4);
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);

	it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorInsertThird) {
	auto it = list.iterator();
	SimpleElement e4;

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.insert(&e4);
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);

	it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorInsertLast) {
	auto it = list.iterator();
	SimpleElement e4;

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
	it.insert(&e4);
	CHECK(it.current() == &e4);
	it.step();


	it = list.iterator();
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == &e4);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorInsertRemoveSame) {
	auto it = list.iterator();

	auto x = it.current();
	it.remove();
	CHECK(it.current() == &e2);
	it.insert(x);

	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}

TEST(NonEmptyLinkedList, IteratorInsertRemoveSwap) {
	auto it = list.iterator();

	auto x = it.current();
	it.remove();
	CHECK(it.current() == &e2);
	it.step();
	CHECK(it.current() == &e1);
	it.insert(x);
	CHECK(it.current() == &e3);
	it.step();
	CHECK(it.current() == &e1);
	it.step();
	CHECK(it.current() == 0);
}
