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

#include "algorithm/IntegerDivision.h"

using namespace pet;

TEST_GROUP(IntDiv)
{
	static inline void test(uint32_t i, uint32_t j) {
		CHECK(pet::uDiv32(i, j) == (i/j));
	}

	static inline void testi(int32_t i, int32_t j) {
		CHECK(pet::iDiv32(i, j) == (i/j));
	}
};

TEST(IntDiv, DivTable10x10)
{
	for(uint32_t i=0; i<10; i++)
		for(uint32_t j=1; j<10; j++)
			test(i, j);
}

TEST(IntDiv, FuckAround)
{
	for(uint32_t i=0; i<10; i++)
		for(uint32_t j=1; j<10; j++)
			test(i*10+j, 10);
}

TEST(IntDiv, Overflow)
{
	for(uint32_t i=0; i<10; i++)
		for(uint32_t j=1; j<10; j++)
			test(0xffffffff-i, 0xff+j);
}

TEST(IntDiv, Signed)
{
	for(int n = -1; n < 2; n += 2)
		for(int m = -1; m < 2; m += 2)
			for(int32_t i=0; i<10; i++)
				for(int32_t j=1; j<10; j++)
					testi(i * n, j * m);
}
