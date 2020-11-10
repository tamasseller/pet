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

#include "pool/Pool.h"

#include "1test/Test.h"

#include "MockAllocator.h"
#include "ubiquitous/General.h"
#include <cstdlib>

using namespace pet;

TEST_GROUP(DynamicPool) {
	constexpr static unsigned int NALLOC = 30;
	constexpr static unsigned int NRUN = 10;
	constexpr static unsigned int countPerFrame = 8;
	unsigned int npointers = 0;
	int *pointers[NALLOC];

	struct InstrumentedDynamicPool: public DynamicPool<int, FailableAllocator, countPerFrame>
	{
		typedef void Frame;
		Frame *getFrameList() {
			return nonemptyFrames.iterator().current();
		}
	};

	TEST_TEARDOWN() {
		CHECK(FailableAllocator::allFreed());
	}
};

TEST(DynamicPool, ZigzagAllocFree) {
	InstrumentedDynamicPool uut;
	int* firstPage[countPerFrame];
	int* secondPage[countPerFrame];

	for(int i=0; i < countPerFrame; i++) {
		auto ret = uut.acquire();

		CHECK(!ret.failed());

		if(!ret)
			return;

		firstPage[i] = ret;
	}

	for(int i=0; i < countPerFrame; i++) {
		auto ret = uut.acquire();

		CHECK(!ret.failed());

		if(!ret)
			return;

		secondPage[i] = ret;
	}

	uut.release(secondPage[0]);
	uut.release(firstPage[0]);

	auto ret = uut.acquire();
	CHECK(!ret.failed());

	auto ret2 = uut.acquire();
	CHECK(!ret2.failed());

	if(firstPage[0] == ret) {
		CHECK(secondPage[0] == ret2);
	} else {
		CHECK(firstPage[0] == ret2);
		CHECK(secondPage[0] == ret);
	}

	for(int i=0; i < countPerFrame; i++) {
		uut.release(firstPage[i]);
		uut.release(secondPage[i]);
	}
}

TEST(DynamicPool, SingleFrameAllocFree) {
	InstrumentedDynamicPool uut;

	CHECK(!uut.getFrameList());

	auto ret = uut.acquire();
	CHECK(!ret.failed());
	pointers[0] = ret;
	CHECK(uut.getFrameList());

	InstrumentedDynamicPool::Frame* frame = uut.getFrameList();

	for(int i=1; i<countPerFrame-1; i++){
		auto ret = uut.acquire();
		CHECK(!ret.failed());
		pointers[i] = ret;
		CHECK(uut.getFrameList());
		CHECK(frame == uut.getFrameList());
	}

	auto ret2 = uut.acquire();
	CHECK(!ret2.failed());
	pointers[countPerFrame-1] = ret2;

	for(int i=countPerFrame-1; i >= 1; i--){
		uut.release(pointers[i]);
		CHECK(uut.getFrameList());
		CHECK(frame == uut.getFrameList());
	}

	if(!pointers[0])
		return;

	uut.release(pointers[0]);
	CHECK(!uut.getFrameList());
}

TEST(DynamicPool, MultiFrameRandomAllocFree) {
	InstrumentedDynamicPool uut;
	unsigned int state = 1245;

	for(int n = 1; n <= NRUN; n++){
		while(npointers<NALLOC){
			auto acq = uut.acquire();
			CHECK(!acq.failed());
			pointers[npointers++] = acq;
		}

		for(int i=0; i<npointers; i++)
			if(pointers[i])
				*pointers[i] = (int)0;

		while(npointers--){
			if(npointers)
			{
				state = state * 1103515245 + 12345;
				int n = (state >> 16) % npointers;
				swap<int*>(pointers[npointers], pointers[n]);
			}
			if(pointers[npointers])
				uut.release(pointers[npointers]);
		}
		npointers = 0;

		CHECK(uut.getFrameList() == 0);
	}
}

TEST(DynamicPool, Destructor) {
	InstrumentedDynamicPool uut;

	for(int i=0; i<countPerFrame-1; i++)
		CHECK(!uut.acquire().failed());
}


TEST_GROUP(PoolInstrumented) {
	template<typename T>
	struct InstrumentedPool: public Pool<T, 4> {
	public:
		InstrumentedPool(): Pool<T, 4>(){}

		int playStory(){
			auto p1 = this->acquire();
			CHECK(!p1.failed());
			this->release(p1);

			//////////////////////////////

			auto p2 = this->acquire();
			CHECK(!p2.failed());

			auto p3 = this->acquire();
			CHECK(!p3.failed());

			//////////////////////////////

			this->release(p2);

			auto p4 = this->acquire();
			CHECK(!p4.failed());

			this->release(p4);
			this->release(p3);

			//////////////////////////////

			auto p5 = this->acquire();
			CHECK(!p5.failed());

			CHECK(!this->acquire().failed());
			CHECK(!this->acquire().failed());
			CHECK(!this->acquire().failed());

			CHECK(this->acquire().failed());
			this->release(p5);

			////////////////////////////////////

			auto p6 = this->acquire();
			CHECK(!p6.failed());
			CHECK(p6 == p5);

			return 0;
		}
	};
};

TEST(PoolInstrumented, OneByte) {
	InstrumentedPool<char>().playStory();
}

TEST(PoolInstrumented, TwoBytes) {
	InstrumentedPool<short>().playStory();
}

TEST(PoolInstrumented, FourBytes) {
	InstrumentedPool<int>().playStory();
}

TEST(PoolInstrumented, EightBytes) {
	InstrumentedPool<long long>().playStory();
}

TEST(PoolInstrumented, ManyBytes) {
	InstrumentedPool<char[128]>().playStory();
}

