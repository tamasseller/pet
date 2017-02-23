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

#include "data/LinkedList.h"

using namespace pet;

TEST_GROUP(FindByField)
{
	struct Element {
		Element* next;
		int integer;
		char character;

		inline Element(int integer, char character):
				integer(integer), character(character), next(0) {}
	};

	LinkedList<Element> list;

	Element *a1, *b2, *c3;

	TEST_SETUP() {
		a1 = new Element(1, 'a');
		b2 = new Element(2, 'b');
		c3 = new Element(3, 'c');
	}

	TEST_TEARDOWN() {
		delete a1;
		delete b2;
		delete c3;
	}
};

TEST(FindByField, Empty) {
	Element *x = list.findByFields(1, &Element::integer);
	CHECK(x == 0);
 }

TEST(FindByField, NonExistent) {
	list.add(a1);

	Element *x = list.findByFields(2, &Element::integer);
	CHECK(x == 0);
}

TEST(FindByField, FindByInteger) {
	list.add(a1);
	list.add(b2);

	Element *x = list.findByFields(2, &Element::integer);
	CHECK(x == b2);

	Element *y = list.findByFields(1, &Element::integer);
	CHECK(y == a1);
}

TEST(FindByField, FindByCharacter) {
	list.add(a1);
	list.add(b2);

	Element *x = list.findByFields('a', &Element::character);
	CHECK(x == a1);

	Element *y = list.findByFields('b', &Element::character);
	CHECK(y == b2);
}

TEST(FindByField, FindByBoth) {
	list.add(a1);
	list.add(b2);
	list.add(c3);

	Element *x = list.findByFields('a', &Element::character);
	CHECK(x == a1);

	Element *y = list.findByFields('b', &Element::character);
	CHECK(y == b2);

	Element *z = list.findByFields(2, &Element::integer);
	CHECK(z == b2);

	Element *w = list.findByFields(3, &Element::integer);
	CHECK(w == c3);
}

TEST_GROUP(FindByFieldPath)
{
	struct Embedded {
		int j;

		struct Nested {
			int k;
			inline Nested (int k): k(k){}
		} nested;

		inline Embedded (int j, int k): j(j), nested(k){}
	};

	struct Element {
		Element* next;

		int i;
		Embedded embedded;
		inline Element (int i, int j, int k): next(0), i(i), embedded(j, k){}
	};

	LinkedList<Element> list;

	Element *a1, *b2, *c3;

	TEST_SETUP() {
		a1 = new Element(1, 2, 3);
		b2 = new Element(2, 3, 4);
		c3 = new Element(3, 4, 5);
	}

	TEST_TEARDOWN() {
		delete a1;
		delete b2;
		delete c3;
	}
};

TEST(FindByFieldPath, Fancy) {
	list.add(a1);
	list.add(b2);
	list.add(c3);

	Element *x = list.findByFields(3, &Element::i);
	CHECK(x == c3);

	Element *y = list.findByFields(3, &Element::embedded, &Embedded::j);
	CHECK(y == b2);

	Element *z = list.findByFields(3, &Element::embedded, &Embedded::nested, &Embedded::Nested::k);
	CHECK(z == a1);
}
