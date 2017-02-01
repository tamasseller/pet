/*
 * TlsfStress.cpp
 *
 *  Created on: 2016.11.15.
 *      Author: tamas.seller
 */

#include "CppUTest/TestHarness.h"

#include "Heap.h"
#include "TlsfPolicy.h"

using namespace mm;

#include "HeapStress.h"

TEST_GROUP(TlsfStress) {
	HeapStress<TlsfHeap<uint32_t, 2, true>, 256*1024, 64, 0, 4096> test;
};

TEST(TlsfStress, AllocFree) {
	test.allocFreeTests();
}

TEST(TlsfStress, AllocShrinkFree) {
	test.allocShrinkFreeTests();
}

