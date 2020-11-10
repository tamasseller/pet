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
#include "1test/FailureInjector.h"

#undef new

#include "pool/Fifo.h"

#include "MockAllocator.h"

using namespace pet;

typedef DynamicFifo<int, FailableAllocator, 3> TestFifo;

TEST_GROUP(DynamicFifo) {
	TestFifo *fifo;

	TEST_SETUP() {
		fifo = new TestFifo;
	}

	TEST_TEARDOWN() {
		delete fifo;
		CHECK_ALWAYS(FailableAllocator::allFreed());
	}
};

TEST(DynamicFifo, Sanity) {
	int ret;
	CHECK(!fifo->readOne(ret));
	CHECK(fifo->writeOne(1));
	CHECK(fifo->readOne(ret));
	CHECK(ret == 1);
	CHECK(!fifo->readOne(ret));
}

TEST(DynamicFifo, WriteReadEmptyStartAgain) {
	int ret;
	CHECK(fifo->writeOne(1));
	CHECK(fifo->readOne(ret));
	CHECK(ret == 1);
	CHECK(!fifo->readOne(ret));

	CHECK(fifo->writeOne(2));
	CHECK(fifo->readOne(ret));
	CHECK(ret == 2);
	CHECK(!fifo->readOne(ret));
}

TEST(DynamicFifo, BulkWriteThenRead) {
	int ret;
	CHECK(fifo->writeOne(1));
	CHECK(fifo->writeOne(2));
	CHECK(fifo->writeOne(3));
	CHECK(fifo->writeOne(4));

	CHECK(fifo->readOne(ret));
	CHECK(ret == 1);

	CHECK(fifo->readOne(ret));
	CHECK(ret == 2);

	CHECK(fifo->readOne(ret));
	CHECK(ret == 3);

	CHECK(fifo->readOne(ret));
	CHECK(ret == 4);

	CHECK(!fifo->readOne(ret));
}

TEST(DynamicFifo, EdgeCase) {
	int ret;
	CHECK(fifo->writeOne(1));
	CHECK(fifo->writeOne(2));
	CHECK(fifo->writeOne(3));

	CHECK(fifo->readOne(ret));
	CHECK(ret == 1);

	CHECK(fifo->readOne(ret));
	CHECK(ret == 2);

	CHECK(fifo->readOne(ret));
	CHECK(ret == 3);

	CHECK(!fifo->readOne(ret));

	CHECK(fifo->writeOne(4));

	CHECK(fifo->readOne(ret));
	CHECK(ret == 4);

	CHECK(!fifo->readOne(ret));
}

TEST(DynamicFifo, MultipleFrames) {
	int ret;

	for(int i=0; i<10; i++)
		CHECK(fifo->writeOne(i));

	for(int i=0; i<10; i++) {
		CHECK(fifo->readOne(ret));
		CHECK(ret == i);
	}

	CHECK(!fifo->readOne(ret));
}

TEST(DynamicFifo, MultpleFramesUnread) {
	int ret;

	for(int i=0; i<10; i++)
		CHECK(fifo->writeOne(i));

}

TEST_GROUP(DynamicFifoClear) {
	TestFifo *fifo;

	TEST_SETUP() {
		fifo = new TestFifo;
	}

	TEST_TEARDOWN() {
		CHECK_ALWAYS(FailableAllocator::allFreed());
		delete fifo;
	}
};

TEST(DynamicFifoClear, CheckIfClearFreesEverything) {
	int ret;

	for(int i=0; i<10; i++)
		CHECK(fifo->writeOne(i));

	fifo->clear();
}

TEST(DynamicFifoClear, CheckIfClearLeavesTheFifoInUsableState) {
	int ret;

	for(int i=0; i<10; i++)
		CHECK(fifo->writeOne(i));

	fifo->clear();

	for(int i=0; i<10; i++)
		CHECK(fifo->writeOne(2*i));

	for(int i=0; i<10; i++) {
		CHECK(fifo->readOne(ret));
		CHECK(ret == 2*i);
	}

	fifo->clear();
}
