/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#include "heap/Heap.h"

#include <stdint.h>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstring>

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#undef new

using namespace mm;

template<class SizeType, unsigned int alignmentBits, unsigned int spare>
class MockPolicy: HeapBase<SizeType, alignmentBits> {
    public:
        typedef HeapBase<SizeType, alignmentBits> Base;

        static constexpr unsigned int freeHeaderSize = spare;

        std::vector<typename Base::Block> freeBlocks;

        void add(typename Base::Block block)
        {
            mock("heapPolicy").actualCall("add").withPointerParameter("block", block.ptr);
            this->freeBlocks.insert(this->freeBlocks.begin(), block);
        }

        void remove(typename Base::Block block)
        {
            mock("heapPolicy").actualCall("remove").withPointerParameter("block", block.ptr);
            for(auto it = freeBlocks.begin(); it != freeBlocks.end(); it++) {
                if((*it).ptr == block.ptr) {
                    this->freeBlocks.erase(it);
                    return;
                }
            }
        }

        typename Base::Block findAndRemove(unsigned int size) {
            mock("heapPolicy").actualCall("findAndRemove").withUnsignedIntParameter("size", size);
            for(auto it = freeBlocks.begin(); it != freeBlocks.end(); it++) {
                if((*it).getSize() >= size) {
                    typename Base::Block ret = *it;
                    this->freeBlocks.erase(it);
                    return ret;
                }
            }

            return 0;
        }

        void update(unsigned int size, typename Base::Block block) {
            mock("heapPolicy").actualCall("update").withPointerParameter("block", block.ptr);
        }
};


template<class SizeType, unsigned int alignmentBits, unsigned int spare>
class HeapTest: HeapHostBase<SizeType, alignmentBits, false> {
	typedef HeapHostBase<SizeType, alignmentBits, false> Base;
	typedef MockPolicy<SizeType, alignmentBits, spare> Policy;
	typedef Heap<Policy, SizeType, alignmentBits> TestHeap;
	using Block = typename Base::Block;

	TestHeap* heap;
	char* base;
	void* start;
	unsigned int maxAlloc;

	unsigned int off(unsigned int size) {
		return Base::Block::headerSize + Base::align((size < Policy::freeHeaderSize) ? Policy::freeHeaderSize : size);
	}

	unsigned int size(unsigned int size) {
		return Base::encode(Base::align((size < Policy::freeHeaderSize) ? Policy::freeHeaderSize : size));
	}

	void* alloc(unsigned int size, bool shouldFail = false) {
		auto ret = heap->alloc(size);
		CHECK(ret.failed() == shouldFail);

		static constexpr uintptr_t mask = (1 << alignmentBits) - 1;
		uintptr_t lowBits = (((uintptr_t)ret) & mask);

		CHECK(lowBits == 0);

		return ret;
	}

    public:
		static constexpr auto minSize = Base::align(Policy::freeHeaderSize ? Policy::freeHeaderSize : 1);

        HeapTest(void* start, unsigned int size):  base((char*)start + Block::headerSize), start(start) {
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap = new TestHeap(start, size);
            maxAlloc = size - Block::headerSize;
        }

        ~HeapTest() {
            delete heap;
        }

        void overSizedFails() {
            void *r = alloc(UINT_MAX, true);
            CHECK(r == 0);
        }

        void normalAlloc() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(100));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(100));
            void *r = alloc(100);
            CHECK(r != 0);
        }

        void smallAlloc() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(1));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(1));
            void *r = alloc(1);
            CHECK(r != 0);
        }

        void bigAlloc() {
        	const unsigned int userSize = 2048 - 2 * sizeof(SizeType) - spare;
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(userSize));
            void *r = alloc(userSize);
            CHECK(r != 0);
        }

        void twoAlloc() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r = alloc(4);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4));
            void *r2 = alloc(4);
            CHECK(r2 != 0);
        }

        void allocFree() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r = alloc(4);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap->free(r);
        }

        void allocFreeForwardMerge() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r = alloc(4);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap->free(r);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r2 = alloc(4);
            CHECK_EQUAL(r, r2);
        }

        void allocFreeNoMerge() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r = alloc(4);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4));
            void *r2 = alloc(4);
            CHECK(r2 != 0);
            CHECK(r2 != r);

            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap->free(r);
        }

        void allocSplitMerge() {
            const unsigned int twoMinSize = Base::align(Block::headerSize) + 2 * Base::align(Policy::freeHeaderSize ? Policy::freeHeaderSize : 1);
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(twoMinSize));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(twoMinSize));
            void *r = alloc(twoMinSize);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(twoMinSize) + off(4));
            void *r2 = alloc(4);
            CHECK(r2 != 0);
            CHECK(r2 != r);

            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap->free(r);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(1));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(1));
            r = alloc(1);
            CHECK(r != 0);
        }

        void allocFreeReverseMerge() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r = alloc(4);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4));
            void *r2 = alloc(4);
            CHECK(r2 != 0);
            CHECK(r2 != r);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4) + off(4));
            void *r3 = alloc(4);
            CHECK(r3 != 0);
            CHECK(r3 != r);
            CHECK(r3 != r2);

            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap->free(r);

            mock("heapPolicy").expectOneCall("update").withPointerParameter("block", base);
            heap->free(r2);
        }

        void allocFreeBothMerge() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4));
            void *r = alloc(4);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4));
            void *r2 = alloc(4);
            CHECK(r2 != 0);
            CHECK(r2 != r);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4) + off(4));
            void *r3 = alloc(4);
            CHECK(r3 != 0);
            CHECK(r3 != r);
            CHECK(r3 != r2);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(4));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4) + off(4) + off(4));
            void *r4 = alloc(4);
            CHECK(r4 != 0);
            CHECK(r4 != r);
            CHECK(r4 != r2);
            CHECK(r4 != r3);

            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
            heap->free(r);

            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(4) + off(4));
            heap->free(r3);

            mock("heapPolicy").expectOneCall("update").withPointerParameter("block", base);
            mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(4) + off(4));
            heap->free(r2);
        }

        void freeCornerCases() {
        	void *d[5];
        	unsigned int big = maxAlloc - 128;
        	unsigned int small = maxAlloc - off(big);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(big));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(big));
        	CHECK((d[0] = alloc(big)) != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(small));
        	CHECK((d[1] = alloc(small)) != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(1));
        	CHECK((alloc(1, true)) == 0);

        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
        	heap->free(d[0]);

        	mock("heapPolicy").expectOneCall("update").withPointerParameter("block", base);
        	heap->free(d[1]);
        }

        void deplete() {
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(1024));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(1024));
            void *r = alloc(1024);
            CHECK(r != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(1024));
            void *r2 = alloc(1024, true);
            CHECK(r2 == 0);
        }

        void shrink() {
        	void *d[5];
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(128));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(128));
        	CHECK((d[0] = alloc(128))  != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(128));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(128) + off(128));
        	CHECK((d[1] = alloc(128))  != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(128));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(128) + off(128) + off(128));
        	CHECK((d[2] = alloc(128))  != 0);

        	CHECK(heap->shrink(d[0], 256) == 128); // Too big not

        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(minSize));
        	CHECK(heap->shrink(d[0], 1) == Base::align(minSize));

        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(128) + off(32));
        	CHECK(heap->shrink(d[1], 32) == 32);

        	mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(128) + off(128) + off(128));
        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(128) + off(128) + off(64));
        	CHECK(heap->shrink(d[2], 64) == 64);

        	mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(minSize));
        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base);
        	heap->free(d[0]);

        	mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(128) + off(32));
        	mock("heapPolicy").expectOneCall("update").withPointerParameter("block", base);
        	heap->free(d[1]);

        	mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(128) + off(128) + off(64));
        	mock("heapPolicy").expectOneCall("update").withPointerParameter("block", base);
        	heap->free(d[2]);
        }

        void shrinkCornerCases() {
        	void *d[5];
        	unsigned int big = maxAlloc - 128;
        	unsigned int small = maxAlloc - off(big);
            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(big));
            mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(big));
        	CHECK((d[0] = alloc(big)) != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(small));
        	CHECK((d[1] = alloc(small)) != 0);

            mock("heapPolicy").expectOneCall("findAndRemove").withUnsignedIntParameter("size", size(1));
        	CHECK((alloc(1, true)) == 0);

        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(128));
        	CHECK(heap->shrink(d[0], 128) == 128);

        	mock("heapPolicy").expectOneCall("remove").withPointerParameter("block", base + off(128));
        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(64));
        	CHECK(heap->shrink(d[0], 64) == 64);

        	CHECK(heap->shrink(d[1], small) == small);

        	mock("heapPolicy").expectOneCall("add").withPointerParameter("block", base + off(big) + off(64));
        	CHECK(heap->shrink(d[1], 64) == 64);
        }
};

#define TEST_TEMPLATE(type, align, spare)                                                                 \
TEST_GROUP(Heap_ ## type ## _ ## align ## _ ## spare) {                                                   \
    typedef HeapTest<type, align, spare> Test;															  \
	Test *test;                                                               							  \
    uint32_t heapSpace[2048 / sizeof(uint32_t)];                                                          \
    TEST_SETUP() {                                                                                        \
        test = new Test((void*)heapSpace, (unsigned int)sizeof(heapSpace));   		                      \
    }                                                                                                     \
                                                                                                          \
    TEST_TEARDOWN() {                                                                                     \
        delete test;                                                                                      \
        mock().checkExpectations();                                                                       \
        mock().clear();                                                                                   \
    }                                                                                                     \
};                                                                                                        \
                                                                                                          \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, OverSizedFails) { test->overSizedFails(); }               \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, NormalAlloc) {test->normalAlloc();}                       \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, typeAlloc) {test->smallAlloc();}                          \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, BigAlloc) {test->bigAlloc();}                             \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, TwoAlloc) {test->twoAlloc();}                             \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, AllocFree) {test->allocFree();}                           \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, AllocFreeForwardMerge) {test->allocFreeForwardMerge();}   \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, AllocFreeNoMerge) {test->allocFreeNoMerge();}             \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, AllocSplitMerge) {test->allocSplitMerge();}               \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, AllocFreeReverseMerge) {test->allocFreeReverseMerge();}   \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, AllocFreeBothMerge) {test->allocFreeBothMerge();}		  \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, FreeCorner) {test->freeCornerCases();}                    \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, Deplete) {test->deplete();}								  \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, Shrink) {test->shrink();}								  \
TEST(Heap_ ## type ## _ ## align ## _ ## spare, ShrinkCorner) {test->shrinkCornerCases();}
