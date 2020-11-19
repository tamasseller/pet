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

#include "data/BinaryTree.h"

using namespace pet;

struct TestTree: BinaryTree {
	struct Node: BinaryTree::Node {
		char data;

		Node(): data('\0') {}
		Node(const char &data): data(data) {}

		void addLeft(Node &c) {
			small = &c;
			c.parent = this;
		}

		void addRight(Node &c) {
			big = &c;
			c.parent = this;
		}

		void cut() {
			if(parent->big == this)
				parent->big = 0;

			if(parent->small == this)
				parent->small = 0;

			parent = 0;
		}
	};

	static void testIterator(Iterator it, const char* pattern) {
		for(; it.current(); it.step())
			CHECK(*pattern++ == ((Node*)it.current())->data);

		CHECK(*pattern == '\0');
	}
};

TEST_GROUP(BinaryTree) {
	TestTree::Node               h;
	TestTree::Node       d,              l;
	TestTree::Node   b,      f,      j,      n;
	TestTree::Node a,  c,  e,  g,  i,  k,  m,  o;

	TEST_SETUP() {
		a = 'a';
		b = 'b';
		c = 'c';
		d = 'd';
		e = 'e';
		f = 'f';
		g = 'g';
		h = 'h';
		i = 'i';
		j = 'j';
		k = 'k';
		l = 'l';
		m = 'm';
		n = 'n';
		o = 'o';

		h.addLeft(d);
		h.addRight(l);

		d.addLeft(b);
		d.addRight(f);

		b.addLeft(a);
		b.addRight(c);

		f.addLeft(e);
		f.addRight(g);

		l.addLeft(j);
		l.addRight(n);

		j.addLeft(i);
		j.addRight(k);

		n.addLeft(m);
		n.addRight(o);
	}
};


TEST(BinaryTree, SubSet) {
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghijklmno");
	TestTree::testIterator(TestTree::Iterator(&b), "bcdefghijklmno");
	TestTree::testIterator(TestTree::Iterator(&c), "cdefghijklmno");
	TestTree::testIterator(TestTree::Iterator(&d), "defghijklmno");
	TestTree::testIterator(TestTree::Iterator(&e), "efghijklmno");
	TestTree::testIterator(TestTree::Iterator(&f), "fghijklmno");
	TestTree::testIterator(TestTree::Iterator(&g), "ghijklmno");
	TestTree::testIterator(TestTree::Iterator(&h), "hijklmno");
	TestTree::testIterator(TestTree::Iterator(&i), "ijklmno");
	TestTree::testIterator(TestTree::Iterator(&j), "jklmno");
	TestTree::testIterator(TestTree::Iterator(&k), "klmno");
	TestTree::testIterator(TestTree::Iterator(&l), "lmno");
	TestTree::testIterator(TestTree::Iterator(&m), "mno");
	TestTree::testIterator(TestTree::Iterator(&n), "no");
	TestTree::testIterator(TestTree::Iterator(&o), "o");
}

TEST(BinaryTree, CutATest){
	a.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "a");
	TestTree::testIterator(TestTree::Iterator(&b), "bcdefghijklmno");
}

TEST(BinaryTree, CutBTest){
	b.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abc");
	TestTree::testIterator(TestTree::Iterator(&d), "defghijklmno");
}

TEST(BinaryTree, CutCTest){
	c.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abdefghijklmno");
	TestTree::testIterator(TestTree::Iterator(&c), "c");
}

TEST(BinaryTree, CutDTest){
	d.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefg");
	TestTree::testIterator(TestTree::Iterator(&h), "hijklmno");
}

TEST(BinaryTree, CutETest){
	e.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdfghijklmno");
	TestTree::testIterator(TestTree::Iterator(&e), "e");
}

TEST(BinaryTree, CutFTest){
	f.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdhijklmno");
	TestTree::testIterator(TestTree::Iterator(&e), "efg");
}

TEST(BinaryTree, CutGTest){
	g.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefhijklmno");
	TestTree::testIterator(TestTree::Iterator(&g), "g");
}

TEST(BinaryTree, CutITest){
	i.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghjklmno");
	TestTree::testIterator(TestTree::Iterator(&i), "i");
}

TEST(BinaryTree, CutJTest){
	j.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghlmno");
	TestTree::testIterator(TestTree::Iterator(&i), "ijk");
}

TEST(BinaryTree, CutKTest){
	k.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghijlmno");
	TestTree::testIterator(TestTree::Iterator(&k), "k");
}

TEST(BinaryTree, CutLTest){
	l.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefgh");
	TestTree::testIterator(TestTree::Iterator(&i), "ijklmno");
}

TEST(BinaryTree, CutMTest){
	m.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghijklno");
	TestTree::testIterator(TestTree::Iterator(&m), "m");
}

TEST(BinaryTree, CutNTest){
	n.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghijkl");
	TestTree::testIterator(TestTree::Iterator(&m), "mno");
}

TEST(BinaryTree, CutOTest){
	o.cut();
	TestTree::testIterator(TestTree::Iterator(&a), "abcdefghijklmn");
	TestTree::testIterator(TestTree::Iterator(&o), "o");
}
