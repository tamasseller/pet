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
	CHECK(list.addFront(&e1));
}

TEST(EmptyDoubleList, RepeatedAdd) {
	CHECK(list.addFront(&e1));
	CHECK(!list.addFront(&e1));
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
	CHECK(list.addFront(&e1));
	CHECK(list.remove(&e1));
}

TEST(EmptyDoubleList, RepeatedRemove) {
	CHECK(list.addFront(&e1));
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

TEST(EmptyDoubleList, Insert) {
	auto it = list.iterator();
	list.insertBefore(&e2, it);			// {} -> {2}
										//  ^

	CHECK(it.current() == nullptr);
	CHECK(list.front() == &e2);
	CHECK(list.back() == &e2);

	auto it2 = list.iterator();			// {2} -> {1, 2}
	list.insertBefore(&e1, it2);		//  ^

	CHECK(it2.current() == &e2);
	CHECK(list.front() == &e1);
	CHECK(list.back() == &e2);
}

TEST(EmptyDoubleList, Insert2) {
	auto it = list.iterator();
	list.insertBefore(&e1, it);			// {} -> {1}
										//  ^

	CHECK(it.current() == nullptr);
	CHECK(list.front() == &e1);
	CHECK(list.back() == &e1);

	list.insertBefore(&e3, it);			// {1} -> {1, 3}
										//   ^

	CHECK(it.current() == nullptr);
	CHECK(list.front() == &e1);
	CHECK(list.back() == &e3);

	auto it2 = list.iterator();			// {1, 3} -> {1, 2, 3}
	it2.step();							//     ^
	list.insertBefore(&e2, it2);

	CHECK(it2.current() == &e3);
	auto it3 = list.iterator();
	CHECK(it3.current() == &e1);
	it3.step();
	CHECK(it3.current() == &e2);
	it3.step();
	CHECK(it3.current() == &e3);
	it3.step();
	CHECK(it3.current() == nullptr);
}

TEST_GROUP(NonEmptyDoubleList) {
	DoubleList<SimpleElement> list;

	TEST_SETUP() {
		CHECK(list.addBack(&e1));
		CHECK(list.addFront(&e2));
		CHECK(list.addFront(&e3));
	}
};

TEST(NonEmptyDoubleList, Pop)
{
	CHECK(list.front() == &e3);
	CHECK(list.back() == &e1);

	CHECK(list.popFront() == &e3);

	CHECK(list.front() == &e2);
	CHECK(list.back() == &e1);

	CHECK(list.popBack() == &e1);

	CHECK(list.front() == &e2);
	CHECK(list.back() == &e2);

	CHECK(list.popFront() == &e2);

	CHECK(list.front() == nullptr);
	CHECK(list.back() == nullptr);

	CHECK(list.popBack() == nullptr);

	CHECK(list.front() == nullptr);
	CHECK(list.back() == nullptr);
}

TEST(NonEmptyDoubleList, PopOtherWay)
{
	CHECK(list.front() == &e3);
	CHECK(list.back() == &e1);

	CHECK(list.popBack() == &e1);

	CHECK(list.front() == &e3);
	CHECK(list.back() == &e2);

	CHECK(list.popFront() == &e3);

	CHECK(list.front() == &e2);
	CHECK(list.back() == &e2);

	CHECK(list.popBack() == &e2);

	CHECK(list.front() == nullptr);
	CHECK(list.back() == nullptr);

	CHECK(list.popFront() == nullptr);

	CHECK(list.front() == nullptr);
	CHECK(list.back() == nullptr);
}

TEST(NonEmptyDoubleList, RemoveNonFirst) {
	CHECK(list.remove(&e2));

	CHECK(!list.addFront(&e1));
	CHECK(list.addFront(&e2));
	CHECK(!list.addFront(&e3));
}

TEST(NonEmptyDoubleList, RemoveNonFirstAddBack) {
	CHECK(list.remove(&e2));

	CHECK(!list.addFront(&e1));
	CHECK(list.addBack(&e2));
	CHECK(!list.addFront(&e3));
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
	CHECK(list.addFront(&e2));

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

TEST_GROUP(DoubleListStress) {
	static constexpr uint32_t prime1 = 449;
	static constexpr uint32_t prime2 = 457;
	static constexpr uint32_t prime3 = 461;
	static constexpr uint32_t prime4 = 463;
    SimpleElement elements[prime4];
    DoubleList<SimpleElement> list;
};

TEST(DoubleListStress, Stress) {
    for(int i=prime1; i; i = ((i + prime1) % prime4)) {
        CHECK(!list.iterator().current());
        for(int j=prime2; j; j = ((j + prime2) % prime4)) {
            unsigned int n = (i + j) % prime4;
            if(i & 1) {
                CHECK(list.addFront(elements + n));
            } else {
                CHECK(list.addBack(elements + n));
            }
        }

        for(int j=1; j<prime4; j++)
            if(j != i)
                CHECK(list.contains(elements + j));

        for(int j=prime3; j; j = (j + prime3) % prime4) {
            unsigned int n = (i + j) % prime4;
            CHECK(list.remove(elements + n));
        }

        CHECK(!list.remove(elements + i % prime4));
        CHECK(!list.remove(elements + (2*i) % prime4));
    }
}
