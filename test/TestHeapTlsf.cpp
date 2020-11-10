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

#include "heap/Heap.h"
#include "heap/TlsfPolicy.h"

using namespace pet;

typedef TlsfPolicy<uint32_t, 2> Policy;
typedef Heap<Policy, uint32_t, 2> TestHeap;

class HeapInternalsTest: private TestHeap::Block {
public:
	using TestHeap::Block::headerSize;
};

class TlsfPolicyInternalsTest {
public:
	using Index = Policy::Index;

	static void indexMapping() {
		Index::Entry m0   = Index::getInsertionEntry(0);
		Index::Entry m1   = Index::getInsertionEntry(1);
		Index::Entry m2   = Index::getInsertionEntry(2);
		Index::Entry m3   = Index::getInsertionEntry(3);
		Index::Entry m4   = Index::getInsertionEntry(4);
		Index::Entry m5   = Index::getInsertionEntry(5);
		Index::Entry m6   = Index::getInsertionEntry(6);
		Index::Entry m7   = Index::getInsertionEntry(7);
		Index::Entry m8   = Index::getInsertionEntry(8);
		Index::Entry m9   = Index::getInsertionEntry(9);
		Index::Entry m14  = Index::getInsertionEntry(14);
		Index::Entry m15  = Index::getInsertionEntry(15);
		Index::Entry m16  = Index::getInsertionEntry(16);
		Index::Entry m17  = Index::getInsertionEntry(17);
		Index::Entry m18  = Index::getInsertionEntry(18);
		Index::Entry m30  = Index::getInsertionEntry(30);
		Index::Entry m31  = Index::getInsertionEntry(31);
		Index::Entry m32  = Index::getInsertionEntry(32);
		Index::Entry m33  = Index::getInsertionEntry(33);
		Index::Entry m34  = Index::getInsertionEntry(34);
		Index::Entry m61  = Index::getInsertionEntry(61);
		Index::Entry m62  = Index::getInsertionEntry(62);
		Index::Entry m63  = Index::getInsertionEntry(63);
		Index::Entry m64  = Index::getInsertionEntry(64);
		Index::Entry m65  = Index::getInsertionEntry(65);
		Index::Entry m66  = Index::getInsertionEntry(66);
		Index::Entry m67  = Index::getInsertionEntry(67);
		Index::Entry m68  = Index::getInsertionEntry(68);
		Index::Entry m69  = Index::getInsertionEntry(69);
		Index::Entry m126 = Index::getInsertionEntry(126);
		Index::Entry m127 = Index::getInsertionEntry(127);
		Index::Entry m128 = Index::getInsertionEntry(128);
		Index::Entry m129 = Index::getInsertionEntry(129);
		Index::Entry m135 = Index::getInsertionEntry(135);
		Index::Entry m136 = Index::getInsertionEntry(136);
		Index::Entry m137 = Index::getInsertionEntry(137);

		CHECK(m0  .fl == 0  && m0  .sl == 0 );
		CHECK(m1  .fl == 0  && m1  .sl == 1 );
		CHECK(m2  .fl == 0  && m2  .sl == 2 );
		CHECK(m3  .fl == 0  && m3  .sl == 3 );
		CHECK(m4  .fl == 0  && m4  .sl == 4 );
		CHECK(m5  .fl == 0  && m5  .sl == 5 );
		CHECK(m6  .fl == 0  && m6  .sl == 6 );
		CHECK(m7  .fl == 0  && m7  .sl == 7 );
		CHECK(m8  .fl == 0  && m8  .sl == 8 );
		CHECK(m9  .fl == 0  && m9  .sl == 9 );
		CHECK(m14 .fl == 0  && m14 .sl == 14);
		CHECK(m15 .fl == 0  && m15 .sl == 15);
		CHECK(m16 .fl == 1  && m16 .sl == 0 );
		CHECK(m17 .fl == 1  && m17 .sl == 1 );
		CHECK(m18 .fl == 1  && m18 .sl == 2 );

		CHECK(m30 .fl == 1  && m30 .sl == 14);
		CHECK(m31 .fl == 1  && m31 .sl == 15);
		CHECK(m32 .fl == 2  && m32 .sl == 0 );
		CHECK(m33 .fl == 2  && m33 .sl == 0 );
		CHECK(m34 .fl == 2  && m34 .sl == 1 );

		CHECK(m61 .fl == 2  && m61 .sl == 14);
		CHECK(m62 .fl == 2  && m62 .sl == 15);
		CHECK(m63 .fl == 2  && m63 .sl == 15);
		CHECK(m64 .fl == 3  && m64 .sl == 0 );
		CHECK(m65 .fl == 3  && m65 .sl == 0 );
		CHECK(m66 .fl == 3  && m66 .sl == 0 );
		CHECK(m67 .fl == 3  && m67 .sl == 0 );
		CHECK(m68 .fl == 3  && m68 .sl == 1 );
		CHECK(m69 .fl == 3  && m69 .sl == 1 );

		CHECK(m126.fl == 3  && m126.sl == 15);
		CHECK(m127.fl == 3  && m127.sl == 15);
		CHECK(m128.fl == 4  && m128.sl == 0 );
		CHECK(m129.fl == 4  && m129.sl == 0 );

		CHECK(m135.fl == 4  && m135.sl == 0 );
		CHECK(m136.fl == 4  && m136.sl == 1 );
		CHECK(m137.fl == 4  && m137.sl == 1 );
	}

	static void indexLookup() {
		Index index;
		Index::Entry s1 = Index::getInsertionEntry(0x1);
		Index::Entry s2 = Index::getInsertionEntry(0x11);
		Index::Entry s3 = Index::getInsertionEntry(0x101);
		Index::Entry s4 = Index::getInsertionEntry(0x1001);
		index.setBits(s1);
		index.setBits(s2);
		index.setBits(s3);
		index.setBits(s4);

		Index::Entry q1 = index.getGreaterEqualEntry(0);
		CHECK(q1.fl == s1.fl && q1.sl == s1.sl);

		Index::Entry q2 = index.getGreaterEqualEntry(1);
		CHECK(q2.fl == s1.fl && q2.sl == s1.sl);

		Index::Entry q3 = index.getGreaterEqualEntry(2);
		CHECK(q3.fl == s2.fl && q3.sl == s2.sl);

		Index::Entry q4 = index.getGreaterEqualEntry(3);
		CHECK(q4.fl == s2.fl && q4.sl == s2.sl);

		Index::Entry q5 = index.getGreaterEqualEntry(0x10);
		CHECK(q5.fl == s2.fl && q5.sl == s2.sl);

		Index::Entry q6 = index.getGreaterEqualEntry(0x11); // in precision range, rounding down is safe
		CHECK(q6.fl == s2.fl && q6.sl == s2.sl);

		Index::Entry q7 = index.getGreaterEqualEntry(0x12);
		CHECK(q7.fl == s3.fl && q7.sl == s3.sl);

		Index::Entry q8 = index.getGreaterEqualEntry(0x100);
		CHECK(q8.fl == s3.fl && q8.sl == s3.sl);

		Index::Entry q9 = index.getGreaterEqualEntry(0x101); // over precision range, rounds up, to be on the safe side
		CHECK(q9.fl == s4.fl && q9.sl == s4.sl);
	}
};

TEST_GROUP(TestHeap) {
	struct Uut: HeapBase<uint32_t, 2> {
		constexpr static unsigned int usedBlockHeaderSize = HeapInternalsTest::headerSize;
		unsigned int data[256*256*8/sizeof(unsigned int)];
		TestHeap heap;
		Uut():heap(data, sizeof(data)) {}

		void *alloc(unsigned int size, bool shouldFail = false) {
			auto ret = heap.alloc(size);
			CHECK(ret.failed() == shouldFail);
			return ret;
		}
	};

	Uut uut;
};

TEST(TestHeap, IndexMapping) {
	TlsfPolicyInternalsTest::indexMapping();
}

TEST(TestHeap, IndexLookup) {
	TlsfPolicyInternalsTest::indexLookup();
}

TEST(TestHeap, AllocSanity) {
	uut.alloc(1);
}

TEST(TestHeap, Deplete) {
	uut.alloc(sizeof(Uut::data)/2+1);
	uut.alloc(sizeof(Uut::data)/2+1, true);
}


TEST(TestHeap, AllocTest1) {
	void* d[5];
	d[0] = uut.alloc(16);
	d[1] = uut.alloc(32);
	d[2] = uut.alloc(16);
	d[3] = uut.alloc(128);
	d[4] = uut.alloc(16);

	uut.heap.free(d[1]);
	uut.heap.free(d[3]);

	d[1] = uut.alloc(30);
	d[3] = uut.alloc(30);

	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
	uut.heap.free(d[2]);
	uut.heap.free(d[3]);
	uut.heap.free(d[4]);
}

TEST(TestHeap, AllocTest2) {
	void *d[2];

	d[0] = uut.alloc(56);
	d[1] = uut.alloc(56);
	uut.heap.free(d[0]);
	CHECK(d[0] == uut.alloc(56));

	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
}

TEST(TestHeap, AllocTest3) {
	void *d[4];

	d[0] = uut.alloc(0x1020 - Uut::usedBlockHeaderSize);
	d[1] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[2] = uut.alloc(0x1010 - Uut::usedBlockHeaderSize);
	d[3] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[0]);
	uut.heap.free(d[2]);

	d[0] = uut.alloc(0x0010 - Uut::usedBlockHeaderSize);
	d[2] = uut.alloc(0x0010 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
	uut.heap.free(d[2]);
	uut.heap.free(d[3]);

}

TEST(TestHeap, AllocTest4) {
	void *d[4];

	d[0] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[1] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[2] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[3] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[0]);
	uut.heap.free(d[2]);

	d[0] = uut.alloc(0x0ff0 - Uut::usedBlockHeaderSize);
	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
	uut.heap.free(d[3]);
}

TEST(TestHeap, FreeTest1) {
	void *d[5];
	d[0] = uut.alloc(0x0010);
	d[1] = uut.alloc(0x1000);
	d[2] = uut.alloc(0x0010);
	d[3] = uut.alloc(0x1000);

	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
	uut.heap.free(d[2]);
	uut.heap.free(d[3]);
}

TEST(TestHeap, FreeTest2) {
	void *d[7];

	d[0] = uut.alloc(0x0100);
	d[1] = uut.alloc(0x1000);

	d[2] = uut.alloc(0x1000);

	d[3] = uut.alloc(0x1000);

	d[4] = uut.alloc(0x0100);
	d[5] = uut.alloc(0x1000);

	d[6] = uut.alloc(0x0100);

	uut.heap.free(d[4]);
	uut.heap.free(d[5]);

	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
	uut.heap.free(d[2]);
	uut.heap.free(d[3]);
	uut.heap.free(d[6]);
}

TEST(TestHeap, ShrinkTest1) {
	void *d[5];
	d[0] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[1] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[2] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);

	uut.heap.shrink(d[0], 0x800 - Uut::usedBlockHeaderSize);
	uut.heap.shrink(d[1], 0xFFF - Uut::usedBlockHeaderSize);
	uut.heap.shrink(d[2], 1);

	uut.heap.free(d[0]);
	uut.heap.free(d[1]);
	uut.heap.free(d[2]);
}

TEST(TestHeap, ShrinkTest2) {
	void *d[4];

	d[0] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[1] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[2] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[3] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[0]);
	uut.heap.free(d[2]);

	uut.heap.shrink(d[1], 0x800 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[1]);
	uut.heap.free(d[3]);
}

TEST(TestHeap, ShrinkTest3) {
	void *d[4];
	d[0] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[1] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[2] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);
	d[3] = uut.alloc(0x1000 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[2]);
	uut.heap.free(d[0]);

	uut.heap.shrink(d[1], 0x800 - Uut::usedBlockHeaderSize);

	uut.heap.free(d[1]);
	uut.heap.free(d[3]);
}
