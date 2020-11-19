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

#undef new

#include "heap/Heap.h"
#include "heap/BestFitPolicy.h"

using namespace pet;

#include "TestHeapStress.h"

#include <iostream>

TEST_GROUP(BestFitStress) {
	HeapStress<BestFitHeap<uint32_t, 2, true>, 512*1024, 64, 0, 4096> test;
};

TEST(BestFitStress, AllocFree) {
	test.allocFreeTests();
}

TEST(BestFitStress, AllocShrinkFree) {
	test.allocShrinkFreeTests();
}
