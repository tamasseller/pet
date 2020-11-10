/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#include "data/BinaryHeap.h"

using namespace pet;

static bool compareNodes(const HeapNode* a, const HeapNode* b);

struct Uut: BinaryHeap<&compareNodes> {
	struct Node: HeapNode {
		char data;

		inline Node(char data): data(data) {}
	};
} uut;

static bool compareNodes(const HeapNode* a, const HeapNode* b)
{
	return static_cast<const Uut::Node*>(a)->data < static_cast<const Uut::Node*>(b)->data;
}


TEST_GROUP(BinaryHeap) {
	Uut uut;
};

TEST(BinaryHeap, Sanity) {
	Uut::Node a('a'), b('b'), c('c');

	CHECK(uut.extreme() == nullptr);

	uut.insert(&c);

	CHECK(uut.extreme() == &c);

	uut.insert(&a);

	CHECK(uut.extreme() == &a);

	uut.insert(&b);

	CHECK(uut.extreme() == &a);
}

TEST(BinaryHeap, Pop) {
	Uut::Node a('a'), b('b'), c('c'), d('d'), e('e'), f('f'), g('g');
	CHECK(uut.extreme() == nullptr);
	uut.insert(&d);
	CHECK(uut.extreme() == &d);
	uut.insert(&e);
	CHECK(uut.extreme() == &d);
	uut.insert(&c);
	CHECK(uut.extreme() == &c);
	uut.insert(&f);
	CHECK(uut.extreme() == &c);
	uut.insert(&b);
	CHECK(uut.extreme() == &b);
	uut.insert(&g);
	CHECK(uut.extreme() == &b);
	uut.insert(&a);
	CHECK(uut.extreme() == &a);

	uut.pop();
	CHECK(uut.extreme() == &b);
	uut.pop();
	CHECK(uut.extreme() == &c);
	uut.pop();
	CHECK(uut.extreme() == &d);
	uut.pop();
	CHECK(uut.extreme() == &e);
	uut.pop();
	CHECK(uut.extreme() == &f);
	uut.pop();
	CHECK(uut.extreme() == &g);
	uut.pop();
	CHECK(uut.extreme() == nullptr);
}

TEST(BinaryHeap, ArbitraryRemove) {
	Uut::Node a('a'), b('b'), c('c'), d('d'), e('e'), f('f'), g('g');
	uut.insert(&d);
	uut.insert(&e);
	uut.insert(&c);
	uut.insert(&f);
	uut.insert(&b);
	uut.insert(&g);
	uut.insert(&a);

	uut.remove(&d);
	CHECK(uut.extreme() == &a);			// a b c e f g

	uut.remove(&a);
	CHECK(uut.extreme() == &b);			// b c e f g

	uut.remove(&f);
	CHECK(uut.extreme() == &b);			// b c e g

	uut.remove(&c);
	CHECK(uut.extreme() == &b);			// b e g

	uut.remove(&b);
	CHECK(uut.extreme() == &e);			// e g

	uut.remove(&e);
	CHECK(uut.extreme() == &g);

	uut.remove(&g);
	CHECK(uut.extreme() == nullptr);
}

TEST(BinaryHeap, MiniStress) {
	Uut::Node nodes[] = {	'p', 'f', 'b', 'n', 'x',
							'e', 'd', 'v', 'y', 'i',
							'k', 'u', 's', 'm', 'o',
							'a', 'z', 'j', 't', 'q',
							'c', 'r', 'l', 'h', 'w', 'g'};

	for(int i = 0; i < sizeof(nodes) / sizeof(nodes[0]); i++)
		uut.insert(nodes + i);

	for(int i = 0; i < 13; i++) {
		Uut::Node* x;
		for(int j = 0; j < sizeof(nodes) / sizeof(nodes[0]); j++)
			if((x = nodes + j)->data == ('a' + i * 2 + 1))
				break;

		uut.remove(x);

		CHECK(static_cast<const Uut::Node*>(uut.extreme())->data == ('a' + i * 2));
		uut.pop();
	}

	CHECK(uut.extreme() == nullptr);
}


TEST(BinaryHeap, Update) {
    Uut::Node a('a'), b('b'), c('c'), d('d'), e('e'), f('f'), g('g');
    uut.insert(&g);
    uut.insert(&b);
    uut.insert(&f);
    uut.insert(&d);
    uut.insert(&a);
    uut.insert(&c);
    uut.insert(&e);

    d.data = 'D';
    uut.update(&d);
    CHECK(uut.extreme() == &d);

    c.data = 'C';
    uut.update(&c);
    CHECK(uut.extreme() == &c);

    e.data = 'E';
    uut.update(&e);
    CHECK(uut.extreme() == &c);

    b.data = 'B';
    uut.update(&b);
    CHECK(uut.extreme() == &b);

    f.data = 'F';
    uut.update(&f);
    CHECK(uut.extreme() == &b);

    a.data = 'A';
    uut.update(&a);
    CHECK(uut.extreme() == &a);

    g.data = 'G';
    uut.update(&g);
    CHECK(uut.extreme() == &a);
}
