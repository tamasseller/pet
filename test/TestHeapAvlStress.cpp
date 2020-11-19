/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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

#undef new

#include "heap/AvlTreePolicy.h"

using namespace pet;

typedef AvlHeap<uint32_t, 2, true> TestHeap;

#include "TestHeapStress.h"

#include <iostream>

struct InstrumentedHeap: public TestHeap {
	using TestHeap::AvlTreePolicy<uint32_t, 2>::HeapBase<uint32_t, 2>::Block;

	using TestHeap::TestHeap;

	void treeDump() {
		for(BinaryTree::Iterator it = this->BinaryTree::iterator(); it.current(); it.step())
			std::cout << Block(it.current()).getSize() << " ";

		std::cout << std::endl;
	}
};

TEST_GROUP(AvlStress) {
	HeapStress<TestHeap, 512*1024, 64, 0, 4096> test;
};

TEST(AvlStress, AllocFree) {
	test.allocFreeTests();
}

TEST(AvlStress, AllocShrinkFree) {
	test.allocShrinkFreeTests();
}

TEST_GROUP(AvlTrace) {
	unsigned int space[1024/sizeof(unsigned int)];
};

TEST(AvlTrace, Trace1) {
	InstrumentedHeap heap;
	heap.init(space, sizeof(space));
	auto x2 = heap.alloc(16);
	CHECK(!x2.failed());
	auto x3 = heap.alloc(16);
	CHECK(!x3.failed());
	auto x4 = heap.alloc(16);
	CHECK(!x4.failed());
	auto x6 = heap.alloc(16);
	CHECK(!x6.failed());
	auto x7 = heap.alloc(16);
	CHECK(!x7.failed());
	heap.free(x6);
	heap.free(x3);
	heap.free(x2);
	heap.free(x7);
}
