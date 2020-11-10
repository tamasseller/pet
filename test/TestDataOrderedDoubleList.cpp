/*******************************************************************************
 *
 * Copyright (c) 2017 Tamás Seller. All rights reserved.
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

#include "data/OrderedDoubleList.h"

using namespace pet;

namespace {
	struct Element {
		Element *next, *prev;
		int data;
		Element(int data): next(0), prev(0), data(data) {}

		inline bool operator < (const Element& big) const {
			return data < big.data;
		}
	};


	Element e1(1), e2(2), e3(3), e4(4), e5(5);
}

TEST_GROUP(OrderedDoubleList) {
	OrderedDoubleList<Element> list;

	template<class... T>
	void expect(T... expected) {
		const int values[] = {expected...};
		auto it = list.iterator();
		for(int i = 0; i<sizeof... (expected); i++) {
			CHECK(it.current() && it.current()->data == values[i]);
			it.step();
		}
		CHECK(!it.current());
	}

	template<class... T>
	void expectPointers(T... expected) {
		const Element* ptrs[] = {expected...};
		auto it = list.iterator();
		for(int i = 0; i<sizeof... (expected); i++) {
			CHECK(it.current() == ptrs[i]);
			it.step();
		}
		CHECK(!it.current());
	}

};

TEST(OrderedDoubleList, OrderedOperations)
{
	CHECK(list.lowest() == nullptr);
	CHECK(list.highest() == nullptr);

	list.add(&e1);

	CHECK(list.lowest() == &e1);
	CHECK(list.highest() == &e1);
	expect(1);

	list.add(&e4);

	CHECK(list.lowest() == &e1);
	CHECK(list.highest() == &e4);
	expect(1, 4);

	list.add(&e2);

	CHECK(list.lowest() == &e1);
	CHECK(list.highest() == &e4);
	expect(1, 2, 4);

	list.add(&e5);

	CHECK(list.lowest() == &e1);
	CHECK(list.highest() == &e5);
	expect(1, 2, 4, 5);

	list.add(&e3);

	CHECK(list.lowest() == &e1);
	CHECK(list.highest() == &e5);
	expect(1, 2, 3, 4, 5);

	CHECK(list.popLowest() == &e1);

	CHECK(list.lowest() == &e2);
	CHECK(list.highest() == &e5);
	expect(2, 3, 4, 5);

	CHECK(list.popHighest() == &e5);

	CHECK(list.lowest() == &e2);
	CHECK(list.highest() == &e4);
	expect(2, 3, 4);

	CHECK(list.popLowest() == &e2);

	CHECK(list.lowest() == &e3);
	CHECK(list.highest() == &e4);
	expect(3, 4);

	CHECK(list.popHighest() == &e4);

	CHECK(list.lowest() == &e3);
	CHECK(list.highest() == &e3);
	expect(3);

	CHECK(list.popLowest() == &e3);

	CHECK(list.lowest() == nullptr);
	CHECK(list.highest() == nullptr);
	expect();

	CHECK(list.popHighest() == nullptr);

	CHECK(list.lowest() == nullptr);
	CHECK(list.highest() == nullptr);
	expect();

}

TEST(OrderedDoubleList, NonOrderedOperations)
{
	list.addLowest(&e2);
	expect(2);

	list.addHighest(&e3);
	expect(2, 3);

	list.addLowest(&e1);
	expect(1, 2, 3);

	list.addHighest(&e4);
	expect(1, 2, 3, 4);

	list.remove(&e2);
	expect(1, 3, 4);

	list.remove(&e4);
	expect(1, 3);

	list.remove(&e1);
	expect(3);

	list.remove(&e3);
	expect();
}

TEST(OrderedDoubleList, EqualElements)
{
	Element a(1), b(2), c(2), d(3), e(3), f(3);

	list.add(&c);
	list.add(&e);
	list.add(&f);

	expectPointers(&c, &e, &f);

	list.add(&a);

	expectPointers(&a, &c, &e, &f);

	list.add(&b);

	expectPointers(&a, &c, &b, &e, &f);

	list.add(&d);

	expectPointers(&a, &c, &b, &e, &f, &d);
}
