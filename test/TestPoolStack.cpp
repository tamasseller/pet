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

#include "pool/Stack.h"

#include "MockAllocator.h"

#include <initializer_list>

using namespace pet;

TEST_GROUP(DynamicStack) {
	struct InstrumentedDynamicStack: public DynamicStack<int, FailableAllocator, 4> {
		typedef DynamicStack<int, FailableAllocator, 4> Base;
		void* getFrame() {
			return currFrame;
		}

		bool fill(const std::initializer_list<int> &list) {
			for(int x: list) {
				if(acquire().failed())
					return false;

				*current() = x;
			}

			return true;
		}
	};

	TEST_TEARDOWN() {
		CHECK_ALWAYS(FailableAllocator::allFreed());
	}
};

TEST(DynamicStack, Sanity) {
	InstrumentedDynamicStack uut;
	CHECK(!uut.current());
	uut.release();
	CHECK(!uut.current());

}

TEST(DynamicStack, HasMore) {
	InstrumentedDynamicStack uut;
	CHECK(!uut.hasMore());

	if(uut.acquire().failed())
		return;

	CHECK(!uut.hasMore());
	CHECK(uut.current());

	if(uut.acquire().failed())
		return;

	CHECK(uut.hasMore());
	uut.release();
}

TEST(DynamicStack, FrameChange) {
	InstrumentedDynamicStack uut;
	if(uut.acquire().failed())
		return;

	void *f = uut.getFrame();

	if(uut.acquire().failed())
		return;

	if(uut.acquire().failed())
		return;

	if(uut.acquire().failed())
		return;

	if(uut.acquire().failed())
		return;

	CHECK(f != uut.getFrame());

	uut.release();
	CHECK(f == uut.getFrame());
	uut.release();
	uut.release();
	uut.release();
	uut.release();
	CHECK(!uut.current());
}

TEST(DynamicStack, Iterator) {
	InstrumentedDynamicStack uut;

	if(!uut.fill({1, 2, 3, 4, 5}))
		return;

	auto i = uut.iterator();
	CHECK(*i.current() == 5);
	i.step();
	CHECK(*i.current() == 4);
	i.step();
	CHECK(*i.current() == 3);
	i.step();
	CHECK(*i.current() == 2);
	i.step();
	CHECK(*i.current() == 1);
	i.step();
	CHECK(!i.current());
	i.step();
	CHECK(!i.current());
}

TEST(DynamicStack, AcquireRelease) {
	InstrumentedDynamicStack uut;

	if(!uut.fill({1, 2, 3, 4, 5}))
		return;

	CHECK(*uut.current() == 5);
	uut.release();
	CHECK(*uut.current() == 4);
	uut.release();
	CHECK(*uut.current() == 3);
	uut.release();
	CHECK(*uut.current() == 2);
	uut.release();
	CHECK(*uut.current() == 1);
	uut.release();
	CHECK(!uut.current());
}

TEST(DynamicStack, BottomUpIterator) {
	InstrumentedDynamicStack uut;

	if(!uut.fill({3, 1, 4, 1, 5, 9, 2, 6, 5, 3}))
		return;

	auto buit = uut.bottomUpiterator();
	CHECK(*buit.current() == 3);
	buit.step(uut);
	CHECK(*buit.current() == 1);
	buit.step(uut);
	CHECK(*buit.current() == 4);
	buit.step(uut);
	CHECK(*buit.current() == 1);
	buit.step(uut);
	CHECK(*buit.current() == 5);
	buit.step(uut);
	CHECK(*buit.current() == 9);
	buit.step(uut);
	CHECK(*buit.current() == 2);
	buit.step(uut);
	CHECK(*buit.current() == 6);
	buit.step(uut);
	CHECK(*buit.current() == 5);
	buit.step(uut);
	CHECK(*buit.current() == 3);
	buit.step(uut);
	CHECK(buit.current() == 0);
}


TEST(DynamicStack, Copy) {
	InstrumentedDynamicStack uut;

	if(!uut.fill({1, 4, 1, 4, 2, 1, 3, 5, 6, 2}))
		return;

	InstrumentedDynamicStack::Base copy;
	if(copy.copyFrom(uut).failed())
		return;

	for(int i=0; i<5; i++)
		uut.release();

	auto copyIt = copy.iterator();
	CHECK(*copyIt.current() == 2); copyIt.step();
	CHECK(*copyIt.current() == 6); copyIt.step();
	CHECK(*copyIt.current() == 5); copyIt.step();
	CHECK(*copyIt.current() == 3); copyIt.step();
	CHECK(*copyIt.current() == 1); copyIt.step();
	CHECK(*copyIt.current() == 2); copyIt.step();
	CHECK(*copyIt.current() == 4); copyIt.step();
	CHECK(*copyIt.current() == 1); copyIt.step();
	CHECK(*copyIt.current() == 4); copyIt.step();
	CHECK(*copyIt.current() == 1); copyIt.step();

	for(int i=0; i<7; i++)
		copy.release();

	auto origIt = uut.iterator();
	CHECK(*origIt.current() == 2); origIt.step();
	CHECK(*origIt.current() == 4); origIt.step();
	CHECK(*origIt.current() == 1); origIt.step();
	CHECK(*origIt.current() == 4); origIt.step();
	CHECK(*origIt.current() == 1); origIt.step();
}

TEST(DynamicStack, Move) {
	InstrumentedDynamicStack uut;

	if(!uut.fill({7, 0, 7, 1, 0, 6}))
		return;

	InstrumentedDynamicStack::Base first;
	first.moveFrom(uut);

	CHECK(uut.current() == 0);

	auto firstIt = first.iterator();
	CHECK(*firstIt.current() == 6); firstIt.step();
	CHECK(*firstIt.current() == 0); firstIt.step();
	CHECK(*firstIt.current() == 1); firstIt.step();
	CHECK(*firstIt.current() == 7); firstIt.step();
	CHECK(*firstIt.current() == 0); firstIt.step();
	CHECK(*firstIt.current() == 7); firstIt.step();

	for(int i=0; i<3; i++)
		first.release();

	InstrumentedDynamicStack::Base second;
	second.moveFrom(first);
		return;

	CHECK(first.current() == 0);

	auto secondIt = first.iterator();
	CHECK(*secondIt.current() == 7); secondIt.step();
	CHECK(*secondIt.current() == 0); secondIt.step();
	CHECK(*secondIt.current() == 7); secondIt.step();
}
