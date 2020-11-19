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


#include "pool/Arena.h"

#include "1test/Test.h"

#include "MockAllocator.h"

using namespace pet;

TEST_GROUP(Arena) {
	TEST_TEARDOWN() {
		CHECK(FailableAllocator::allFreed());
	}
};

TEST(Arena, Free1) {
	Arena<int, FailableAllocator, 64> uut;

	for(int i=0; i<448; i++)
		CHECK(!uut.acquire().failed());
}

TEST(Arena, Free2) {
	Arena<int, FailableAllocator, 32> uut;

	for(int i=0; i<448; i++)
		CHECK(!uut.acquire().failed());
}

TEST(Arena, Free3) {
	Arena<int, FailableAllocator, 13> uut;

	for(int i=0; i<403; i++)
		CHECK(!uut.acquire().failed());
}
