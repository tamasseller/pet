/*
 * HeapStress.h
 *
 *  Created on: 2016.11.15.
 *      Author: tamas.seller
 */

#ifndef HEAPSTRESS_H_
#define HEAPSTRESS_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Trace.h"

class HeapStressTestTraceTag;

template <class Heap, unsigned int heapSize, unsigned int rounds, unsigned int minAlloc, unsigned int maxAlloc>
class HeapStress: ubiq::Trace<HeapStressTestTraceTag> {
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

	unsigned int seed = 1234;
	unsigned int random(unsigned int l, unsigned int h) {
		if(h > l)
			return l + rand_r(&seed) % (h-l);

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
			int n = random(0, db.getSize());
			amount -= db[n].size;
			heap.free(db[n].ptr);
			CHECK(heap.dump(data));
			db.remove(n);
		};
	}

	void random_shrink(int amount){
		while(amount > 0){
			int n = random(0, db.getSize());
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
