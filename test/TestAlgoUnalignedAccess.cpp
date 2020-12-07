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

#include "algorithm/UnalignedAccess.h"

#include "1test/Test.h"


TEST_GROUP(UnalignedAccess){};

TEST(UnalignedAccess, ReadWriteInt)
{
	char buffer[sizeof(int) * 2 - 1];

	for(auto i = 0u; i < sizeof(int); i++)
	{
		int testValues[] = {0x12345678, 0x5a55a5aa, 0x1337c0de};

		pet::unalignedWrite<int>(buffer + i, 0);
		for(const auto &v: testValues)
		{
			pet::unalignedWrite<int>(buffer + i, v);
			CHECK(pet::unalignedRead<int>(buffer + i) == v);
		}
	}
}

TEST(UnalignedAccess, ReadWriteShort)
{
	char buffer[sizeof(short) * 2 - 1];

	for(auto i = 0u; i < sizeof(short); i++)
	{
		short testValues[] = {0x1234, 0x5a55, 0x1337};

		pet::unalignedWrite<short>(buffer + i, 0);
		for(const auto &v: testValues)
		{
			pet::unalignedWrite<short>(buffer + i, v);
			CHECK(pet::unalignedRead<short>(buffer + i) == v);
		}
	}
}
