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

#ifndef MOCKALLOCATOR_H_
#define MOCKALLOCATOR_H_

#include <malloc.h>

#include "1test/FailureInjector.h"
#include "ubiquitous/Error.h"
#include "ubiquitous/Trace.h"

class MockAllocatorTrace: public pet::Trace<MockAllocatorTrace> {};

struct Allocator {
	static unsigned int count;

	static void* alloc(unsigned int s) {
		void *ret = malloc(s);
		count++;

		MockAllocatorTrace::info() << "alloc   " << s << " -> " << ret << "\n";

		return ret;
	}

	static void free(void* p) {
		count--;

		MockAllocatorTrace::info() << "free   " << p << "\n";

		return ::free(p);
	}

	static unsigned int shrink(void* p, unsigned int s) {
		MockAllocatorTrace::info() << "shrink " << p << " to " << s << "\n";
		return s;
	}

	static bool allFreed() {
	    bool ret = count == 0;
	    count = 0;
		return ret;
	}

	static inline void traceReferenceAcquistion(...) {}
	static inline void traceReferenceRelease(...) {}
};

struct FailableAllocator: public Allocator {
	virtual const char* getFailureSourceName() {
		return "Allocator";
	}

	static void* alloc(unsigned int s) {
		if(pet::FailureInjector::shouldSimulateError())
			return 0;

		return Allocator::alloc(s);
	}
};

#endif /* MOCKALLOCATOR_H_ */
