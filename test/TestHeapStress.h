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

#ifndef HEAPSTRESS_H_
#define HEAPSTRESS_H_

#include <cstdio>
#include <cstring>

#include "ubiquitous/Trace.h"

class HeapStressTestTraceTag;

template <class Heap, unsigned int heapSize, unsigned int rounds, unsigned int minAlloc, unsigned int maxAlloc>
class HeapStress: pet::Trace<HeapStressTestTraceTag> {
	Heap heap;
	uint32_t data[heapSize / sizeof(uint32_t)];

	struct BlockDb {
		struct Entry {
			void *ptr;
			unsigned int size;

			Entry() = default;
			inline Entry(void *ptr, unsigned int size): ptr(ptr), size(size) {}
		};

		Entry entries[heapSize];
		unsigned int idx = 0;

		void add(const Entry &e) {
			entries[idx++] = e;
		}

		Entry &operator[](unsigned int i) {
			return entries[i];
		}

		void remove(int i)
		{
			entries[i] = entries[--idx];
		}

		bool isEmpty() {
			return !idx;
		}

		unsigned int getSize() {
			return idx;
		}
	};

	BlockDb db;

	uint32_t state = 1234;
	unsigned int random(unsigned int l, unsigned int h) 
	{
		if(h > l) 
		{
		    state = state * 1103515245 + 12345;
			return l + ((state >> 16) % (h - l + 1));
		}
		
		return 0;
	}

	void random_fill(){
		while(1){
			unsigned int blockSize = random(minAlloc, maxAlloc);

			auto ptr = heap.alloc(blockSize);
			CHECK(heap.dump(data));

			if(ptr.failed())
				break;

			memset(ptr, 0x5a, blockSize);
			db.add(typename BlockDb::Entry(ptr, blockSize));
		};
	}

	void random_free(int amount){
		while(!db.isEmpty() && (amount > 0)){
			int n = random(0, db.getSize() - 1);
			amount -= db[n].size;
			heap.free(db[n].ptr);
			CHECK(heap.dump(data));
			db.remove(n);
		};
	}

	void random_shrink(int amount){
		while(amount > 0){
			int n = random(0, db.getSize() - 1);
			int blockSize = random(minAlloc, db[n].size);
			unsigned int oldSize = db[n].size;
			db[n].size = heap.shrink(db[n].ptr, blockSize);
			CHECK(heap.dump(data));
			amount -= oldSize - db[n].size;
		};
	}

public:
	HeapStress(): heap(data, sizeof(data)) {}

	int allocFreeTests(){
		for(int i=0; i<rounds; i++){
			random_fill();
			random_free(heapSize/2);
		}

		random_free(heapSize);
		return 0;
	}

	int allocShrinkFreeTests(){
		for(int i=0; i<rounds; i++){
			random_fill();
			random_shrink(heapSize/10);
			random_free(heapSize/3);
		}
		return 0;
	}
};

#endif /* HEAPSTRESS_H_ */
