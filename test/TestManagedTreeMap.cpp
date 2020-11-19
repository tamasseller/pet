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

#include "managed/TreeMap.h"
#include "MockAllocator.h"

using namespace pet;

typedef DynamicTreeMap<int, int, FailableAllocator> TestMap;

TEST_GROUP(TreeMapHeadless) {
};

TEST(TreeMapHeadless, Destructor) {
	{
		TestMap map;
		map.put(3,1);
		map.put(2,1);
		map.put(4,1);
		map.put(1,1);
		map.put(5,1);
	}
	CHECK(Allocator::allFreed());
}


TEST_GROUP(TreeMapEmtpy) {
	TestMap *map;

	TEST_SETUP() {
		map = new TestMap;
	}

	TEST_TEARDOWN() {
		delete map;
		CHECK(Allocator::allFreed());
	}
};

TEST(TreeMapEmtpy, AddOne) {
	map->put(1, 2);
	CHECK(map->get(1));
	auto x = map->get(1);
	if(x)
	CHECK(*x == 2);
}

TEST(TreeMapEmtpy, UpdateOne) {
	map->put(1, 2);
	CHECK(map->get(1));
	auto x = map->get(1);
	if(x)
		CHECK(*x == 2);

	map->put(1, 3);
	CHECK(map->get(1));
	CHECK(*map->get(1) == 3);
}

TEST(TreeMapEmtpy, RemoveOne) {
	map->put(1, 2);
	CHECK(map->get(1));
	map->remove(1);
	CHECK(!map->get(1));
}

TEST(TreeMapEmtpy, RemoveNone) {
	map->remove(1);
	map->put(1, 2);
	CHECK(map->get(1));
}

TEST_GROUP(TreeMapNormalize) {
	TestMap *map;

	TEST_SETUP() {
		map = new TestMap;
		map->put(10, 0);
	}

	TEST_TEARDOWN() {
		CHECK(map->get(10));
		auto x = map->get(10);
		if(x)
			CHECK(*x == 0);
		delete map;
		CHECK(Allocator::allFreed());
	}
};

TEST(TreeMapNormalize, RotateSmall) {
	map->put(9, 0);
	map->put(8, 0);
}

TEST(TreeMapNormalize, RotateSmallWithBig) {
	map->put(11, 0);
	map->put(8, 0);
	map->put(9, 0);
	map->put(6, 0);
	map->put(7, 0);
}

TEST(TreeMapNormalize, RotateBig) {
	map->put(11, 0);
	map->put(12, 0);
}

TEST(TreeMapNormalize, RotateBigWithSmall) {
	map->put(9, 0);
	map->put(12, 0);
	map->put(11, 0);
	map->put(14, 0);
	map->put(13, 0);
}

TEST(TreeMapNormalize, DoubleRotateSmallBig) {
	map->put(8, 0);
	map->put(9, 0);
}

TEST(TreeMapNormalize, DoubleRotateBigSmall) {
	map->put(12, 0);
	map->put(11, 0);
}


TEST_GROUP(TreeMapSimpleRemove) {
	TestMap *map;

	TEST_SETUP() {
		map = new TestMap;
		map->put(4, 0);
		map->put(2, 0);
		map->put(6, 0);
		map->put(1, 0);
		map->put(3, 0);
		map->put(5, 0);
		map->put(7, 0);
	}

	TEST_TEARDOWN() {
		int n = 0;

		for(auto it = map->iterator(); it.currentKey(); it.step())
			n++;

		CHECK(n == 6);

		delete map;
		CHECK(Allocator::allFreed());
	}
};

TEST(TreeMapSimpleRemove, Remove1) {
	map->remove(1);
}

TEST(TreeMapSimpleRemove, Remove2) {
	map->remove(2);
}

TEST(TreeMapSimpleRemove, Remove3) {
	map->remove(3);
}

TEST(TreeMapSimpleRemove, Remove4) {
	map->remove(4);
}

TEST(TreeMapSimpleRemove, Remove5) {
	map->remove(5);
}

TEST(TreeMapSimpleRemove, Remove6) {
	map->remove(6);
}

TEST(TreeMapSimpleRemove, Remove7) {
	map->remove(7);
}

TEST_GROUP(TreeMapTrickyRemove) {
	TestMap *map;

	TEST_SETUP() {
		map = new TestMap;
		map->put(4, 0);
		map->put(2, 0);
		map->put(6, 0);
	}

	TEST_TEARDOWN() {
		int n = 0;

		for(auto it = map->iterator(); it.currentKey(); it.step())
			n++;

		CHECK(n == 3);

		delete map;
		CHECK(Allocator::allFreed());
	}
};

TEST(TreeMapTrickyRemove, Add1Remove2) {
	map->put(1, 0);
	map->remove(2);
}

TEST(TreeMapTrickyRemove, Add1Remove4) {
	map->put(1, 0);
	map->remove(4);
}

TEST(TreeMapTrickyRemove, Add1Remove6) {
	map->put(1, 0);
	map->remove(6);
}

TEST(TreeMapTrickyRemove, Add3Remove2) {
	map->put(3, 0);
	map->remove(2);
}

TEST(TreeMapTrickyRemove, Add3Remove4) {
	map->put(3, 0);
	map->remove(4);
}

TEST(TreeMapTrickyRemove, Add3Remove6) {
	map->put(3, 0);
	map->remove(6);
}

TEST_GROUP(TreeMapVeryTrickyRemove) {
	TestMap *map;

	TEST_SETUP() {
		map = new TestMap;
		map->put(5, 0);
		map->put(2, 0);
		map->put(7, 0);
		map->put(1, 0);
		map->put(4, 0);
		map->put(6, 0);
		map->put(3, 0);
	}

	TEST_TEARDOWN() {
		int n = 0;

		for(auto it = map->iterator(); it.currentKey(); it.step())
			n++;

		CHECK(n == 6);

		delete map;
		CHECK(Allocator::allFreed());
	}
};

TEST(TreeMapVeryTrickyRemove, LastLine) {
	map->remove(5);
}


TEST_GROUP(TreeMapSquares) {
	TestMap *map;

	TEST_SETUP() {
		map = new TestMap;
		for(int i = 0; i<100; i++)
			map->put(i, i*i);
	}

	TEST_TEARDOWN() {
		for(int i = 149; i >= 0; i--)
			map->remove(i);

		delete map;
		CHECK(Allocator::allFreed());
	}
};

TEST(TreeMapSquares, Contains) {
	for(int i = 0; i<100; i++)
		CHECK(map->contains(i));
}

TEST(TreeMapSquares, NotContains) {
	for(int i = 100; i<150; i++)
		CHECK(!map->contains(i));
}

TEST(TreeMapSquares, RemoveOdd) {
	for(int i = 1; i<100; i += 2)
		map->remove(i);

	for(int i = 1; i<100; i += 2)
		CHECK(!map->contains(i));
}

TEST(TreeMapSquares, RemoveOddThenEven) {
	for(int i = 1; i<100; i += 2)
		map->remove(i);

	for(int i = 0; i<100; i += 2)
		map->remove(i);

	for(int i = 0; i<100; i++)
		CHECK(!map->contains(i));
}

TEST(TreeMapSquares, Update) {
	for(int i = 0; i<100; i += 2)
		map->put(i, i);

	for(int i = 0; i<100; i++)
		CHECK(*map->get(i) == ((i % 2) ? i*i : i));
}

TEST(TreeMapSquares, AddMore) {
	for(int i = 100; i<150; i++)
		map->put(i, i*i);

	for(int i = 0; i<150; i++) {
		auto x = map->get(i);
		if(x)
			CHECK(*x == i*i);
	}
}

TEST(TreeMapSquares, Iteratate) {
	int idx = 0;
	auto it = map->iterator();
	for(; it.currentKey(); it.step()) {
		CHECK(*it.currentKey() == idx);
		CHECK(*it.currentValue() == idx*idx);
		idx++;
	}

	CHECK(100 == idx);

	CHECK(it.currentValue() == 0);
	it.step();
	CHECK(it.currentKey() == 0);
	CHECK(it.currentValue() == 0);
}
