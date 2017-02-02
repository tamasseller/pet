/*
 * TlsfStress.cpp
 *
 *  Created on: 2016.11.15.
 *      Author: tamas.seller
 */

#include "CppUTest/TestHarness.h"

#undef new

#include "heap/AvlTreePolicy.h"

using namespace mm;

typedef AvlHeap<uint32_t, 2, true> TestHeap;

#include "TestHeapStress.h"

#include <iostream>

struct IntrumentedHeap: public TestHeap {
	using TestHeap::AvlTreePolicy<uint32_t, 2>::HeapBase<uint32_t, 2>::Block;

	IntrumentedHeap(void* space, unsigned int size): Heap(space, size) {}

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
	IntrumentedHeap heap(space, sizeof(space));
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
