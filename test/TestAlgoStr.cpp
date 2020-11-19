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

#include "algorithm/Str.h"

#include <string.h>
#include <stdint.h>

using namespace pet;

TEST_GROUP(Str)
{
	constexpr static const char *str1 = "asd";
	constexpr static const char *str2 = "qwe";
	constexpr static const char *str3 = "asdqwe";
	constexpr static const char *str4 = "qweasd";

	char mstr1[strlen(str1)+1];
	char mstr2[strlen(str2)+1];
	char mstr3[strlen(str3)+1];
	char mstr4[strlen(str4)+1];

	char temp1[strlen(str1)+1];
	char temp2[strlen(str2)+1];
	char temp3[strlen(str3)+1];
	char temp4[strlen(str4)+1];

	TEST_SETUP() {
		for(int i=0; i<strlen(str1)+1; i++) {
			mstr1[i] = str1[i];
			temp1[i] = '\0';
		}

		for(int i=0; i<strlen(str2)+1; i++) {
			mstr2[i] = str2[i];
			temp2[i] = '\0';
		}

		for(int i=0; i<strlen(str3)+1; i++) {
			mstr3[i] = str3[i];
			temp3[i] = '\0';
		}

		for(int i=0; i<strlen(str4)+1; i++) {
			mstr4[i] = str4[i];
			temp4[i] = '\0';
		}
	}
};

TEST(Str, CmpEquals) {
	CHECK(Str::cmp(mstr1, str1) == 0);
	CHECK(Str::cmp(str1, mstr1) == 0);

	CHECK(Str::cmp(mstr2, str2) == 0);
	CHECK(Str::cmp(str2, mstr2) == 0);

	CHECK(Str::cmp(mstr3, str3) == 0);
	CHECK(Str::cmp(str3, mstr3) == 0);

	CHECK(Str::cmp(mstr4, str4) == 0);
	CHECK(Str::cmp(str4, mstr4) == 0);
}

TEST(Str, CmpOrder) {
	CHECK(Str::cmp(mstr1, mstr2) < 0); // asd < qwe
	CHECK(Str::cmp(mstr2, mstr3) > 0); // qwe > asdqwe
	CHECK(Str::cmp(mstr3, mstr4) < 0); // asdqwe < qweasd
	CHECK(Str::cmp(mstr4, mstr1) > 0); // qweasd > asd

	CHECK(Str::cmp(mstr1, mstr3) < 0); // asd < asdqwe
	CHECK(Str::cmp(mstr2, mstr4) < 0); // qwe < qweasd}
}

TEST(Str, nCmpEquals) {
	CHECK(Str::ncmp(mstr1, str1, 10) == 0);
	CHECK(Str::ncmp(str1, mstr1, 10) == 0);

	CHECK(Str::ncmp(mstr2, str2, 10) == 0);
	CHECK(Str::ncmp(str2, mstr2, 10) == 0);

	CHECK(Str::ncmp(mstr3, str3, 10) == 0);
	CHECK(Str::ncmp(str3, mstr3, 10) == 0);

	CHECK(Str::ncmp(mstr4, str4, 10) == 0);
	CHECK(Str::ncmp(str4, mstr4, 10) == 0);
}


TEST(Str, nCmpOrder) {
	CHECK(Str::ncmp(mstr1, mstr2, 10) < 0); // asd < qwe
	CHECK(Str::ncmp(mstr2, mstr3, 10) > 0); // qwe > asdqwe
	CHECK(Str::ncmp(mstr3, mstr4, 10) < 0); // asdqwe < qweasd
	CHECK(Str::ncmp(mstr4, mstr1, 10) > 0); // qweasd > asd

	CHECK(Str::ncmp(mstr1, mstr3, 10) < 0); // asd < asdqwe
	CHECK(Str::ncmp(mstr2, mstr4, 10) < 0); // qwe < qweasd

	CHECK(Str::ncmp(mstr1, mstr2, 4) < 0); // asd < qwe
	CHECK(Str::ncmp(mstr2, mstr3, 4) > 0); // qwe > asdq
	CHECK(Str::ncmp(mstr3, mstr4, 4) < 0); // asdq < qwea
	CHECK(Str::ncmp(mstr4, mstr1, 4) > 0); // qwea > asd

	CHECK(Str::ncmp(mstr1, mstr3, 4) < 0); // asd < asdq
	CHECK(Str::ncmp(mstr2, mstr4, 4) < 0); // qwe < qwea

	CHECK(Str::ncmp(mstr1, mstr2, 3) < 0); // asd < qwe
	CHECK(Str::ncmp(mstr2, mstr3, 3) > 0); // qwe > asd
	CHECK(Str::ncmp(mstr3, mstr4, 3) < 0); // asd < qwe
	CHECK(Str::ncmp(mstr4, mstr1, 3) > 0); // qwe > asd

	CHECK(Str::ncmp(mstr1, mstr3, 3) == 0); // asd = asd
	CHECK(Str::ncmp(mstr2, mstr4, 3) == 0); // qwe = qwe
}

TEST(Str, cpy) {
	Str::cpy(temp1, mstr1);
	CHECK(Str::cmp(temp1, str1) == 0);

	Str::cpy(temp2, mstr2);
	CHECK(Str::cmp(temp2, str2) == 0);

	Str::cpy(temp3, mstr3);
	CHECK(Str::cmp(temp3, str3) == 0);

	Str::cpy(temp4, mstr4);
	CHECK(Str::cmp(temp4, str4) == 0);
}

TEST(Str, nCpy) {
		Str::ncpy(temp1, mstr1, 2);
		CHECK(Str::ncmp(temp1, str1, 2) == 0);
		CHECK(strlen(temp1) == 2);

		Str::ncpy(temp2, mstr2, 2);
		CHECK(Str::ncmp(temp2, str2, 2) == 0);
		CHECK(strlen(temp2) == 2);

		Str::ncpy(temp3, mstr3, 2);
		CHECK(Str::ncmp(temp3, str3, 2) == 0);
		CHECK(strlen(temp3) == 2);

		Str::ncpy(temp4, mstr4, 2);
		CHECK(Str::ncmp(temp4, str4, 2) == 0);
		CHECK(strlen(temp4) == 2);
}

TEST(Str, ntCpy) {
	Str::ncpy(temp1, mstr1, 3);
	Str::ntcpy(temp1, mstr1, 2);
	CHECK(Str::ncmp(temp1, str1, 2) == 0);
	CHECK(strlen(temp1) == 3);

	Str::ncpy(temp2, mstr2, 3);
	Str::ntcpy(temp2, mstr2, 2);
	CHECK(Str::ncmp(temp2, str2, 2) == 0);
	CHECK(strlen(temp2) == 3);

	Str::ncpy(temp3, mstr3, 3);
	Str::ntcpy(temp3, mstr3, 2);
	CHECK(Str::ncmp(temp3, str3, 2) == 0);
	CHECK(strlen(temp3) == 3);

	Str::ncpy(temp4, mstr4, 3);
	Str::ntcpy(temp4, mstr4, 2);
	CHECK(Str::ncmp(temp4, str4, 2) == 0);
	CHECK(strlen(temp4) == 3);
}

TEST(Str, nLength) {
	CHECK(Str::nLength(temp1, 10) == 0);
	CHECK(Str::nLength(temp1, 10) == 0);
	CHECK(Str::nLength(temp1, 10) == 0);
	CHECK(Str::nLength(temp1, 10) == 0);

	Str::ncpy(temp1, mstr1, 2);
	Str::ncpy(temp2, mstr2, 2);
	Str::ncpy(temp3, mstr3, 2);
	Str::ncpy(temp4, mstr4, 2);

	CHECK(Str::nLength(temp1, 10) == 2);
	CHECK(Str::nLength(temp2, 10) == 2);
	CHECK(Str::nLength(temp3, 10) == 2);
	CHECK(Str::nLength(temp4, 10) == 2);

	CHECK(Str::nLength(temp1, 2) == 2);
	CHECK(Str::nLength(temp2, 2) == 2);
	CHECK(Str::nLength(temp3, 2) == 2);
	CHECK(Str::nLength(temp4, 2) == 2);

	CHECK(Str::nLength(temp1, 1) == 1);
	CHECK(Str::nLength(temp2, 1) == 1);
	CHECK(Str::nLength(temp3, 1) == 1);
	CHECK(Str::nLength(temp4, 1) == 1);
}

TEST(Str, utoa10) {
	char temp[12];

	CHECK(Str::utoa<10>(0, temp, sizeof(temp)));
	CHECK(strcmp(temp, "0") == 0);

	CHECK(Str::utoa<10>(1, temp, sizeof(temp)));
	CHECK(strcmp(temp, "1") == 0);

	CHECK(Str::utoa<10>(10, temp, sizeof(temp)));
	CHECK(strcmp(temp, "10") == 0);

	CHECK(Str::utoa<10>(11, temp, sizeof(temp)));
	CHECK(strcmp(temp, "11") == 0);

	CHECK(Str::utoa<10>(23, temp, sizeof(temp)));
	CHECK(strcmp(temp, "23") == 0);

	CHECK(Str::utoa<10>(99, temp, sizeof(temp)));
	CHECK(strcmp(temp, "99") == 0);

	CHECK(Str::utoa<10>(100, temp, sizeof(temp)));
	CHECK(strcmp(temp, "100") == 0);

	CHECK(Str::utoa<10>(654, temp, sizeof(temp)));
	CHECK(strcmp(temp, "654") == 0);

	CHECK(!Str::utoa<10>(999999, temp, 0));
	CHECK(!Str::utoa<10>(999999, temp, 1));
	CHECK(!Str::utoa<10>(999999, temp, 2));
	CHECK(!Str::utoa<10>(999999, temp, 3));
	CHECK(!Str::utoa<10>(999999, temp, 4));
	CHECK(!Str::utoa<10>(999999, temp, 5));
	CHECK(!Str::utoa<10>(999999, temp, 6));
	CHECK(Str::utoa<10>(999999, temp, 7));
	CHECK(strcmp(temp, "999999") == 0);
}

TEST(Str, utoa13) {
	char temp[12];

	CHECK(Str::utoa<13>(13, temp, sizeof(temp)));
	CHECK(strcmp(temp, "10") == 0);

	CHECK(Str::utoa<13>(14, temp, sizeof(temp)));
	CHECK(strcmp(temp, "11") == 0);

	CHECK(Str::utoa<13>(25, temp, sizeof(temp)));
	CHECK(strcmp(temp, "1c") == 0);

	CHECK(Str::utoa<13>(26, temp, sizeof(temp)));
	CHECK(strcmp(temp, "20") == 0);

	CHECK(Str::utoa<13>(168, temp, sizeof(temp)));
	CHECK(strcmp(temp, "cc") == 0);

	CHECK(Str::utoa<13>(169, temp, sizeof(temp)));
	CHECK(strcmp(temp, "100") == 0);
}

TEST(Str, utoa2) {
	char temp[12];

	CHECK(Str::utoa<2>(5, temp, sizeof(temp)));
	CHECK(strcmp(temp, "101") == 0);

	CHECK(Str::utoa<2>(3, temp, sizeof(temp)));
	CHECK(strcmp(temp, "11") == 0);

	CHECK(Str::utoa<2>(5, temp, sizeof(temp)));
	CHECK(strcmp(temp, "101") == 0);

	CHECK(Str::utoa<2>(9, temp, sizeof(temp)));
	CHECK(strcmp(temp, "1001") == 0);

	CHECK(Str::utoa<2>(180, temp, sizeof(temp)));
	CHECK(strcmp(temp, "10110100") == 0);
}

TEST(Str, utoa16) {
	char temp[12];

	CHECK(Str::utoa<16>(4919, temp, sizeof(temp)));
	CHECK(strcmp(temp, "1337") == 0);

	CHECK(Str::utoa<16>(721077, temp, sizeof(temp)));
	CHECK(strcmp(temp, "b00b5") == 0);

	CHECK(Str::utoa<16>(24589, temp, sizeof(temp)));
	CHECK(strcmp(temp, "600d") == 0);

	char big[sizeof(uint32_t)*2+1];
	CHECK(Str::utoa<16>(UINT_MAX, big, sizeof(big)));
	CHECK(strcmp(big, "ffffffff") == 0);
}

TEST(Str, atou10) {
	unsigned int x;

	CHECK(Str::atou<10>(x, "1", 10));
	CHECK(x == 1);

	CHECK(Str::atou<10>(x, "10", 10));
	CHECK(x == 10);

	CHECK(Str::atou<10>(x, "100", 10));
	CHECK(x == 100);

	CHECK(Str::atou<10>(x, "1000", 10));
	CHECK(x == 1000);

	CHECK(Str::atou<10>(x, "1414213562", 10));
	CHECK(x == 1414213562);
}


TEST(Str, atou16) {
	unsigned int x;

	CHECK(Str::atou<16>(x, "1Ff", 1));
	CHECK(x == 1);

	CHECK(Str::atou<16>(x, "1Ff", 1000));
	CHECK(x == 511);

	CHECK(!Str::atou<16>(x, "", 1));
	CHECK(!Str::atou<16>(x, "x", 1));
	CHECK(!Str::atou<16>(x, "x", 1000));

	CHECK(Str::atou<16>(x, "101", 2));
	CHECK(x == 16);

	CHECK(Str::atou<16>(x, "101", 100));
	CHECK(x == 257);

	CHECK(Str::atou<16>(x, "400", 3));
	CHECK(x == 1024);
}

TEST(Str, itoa) {
	char temp[12];

	CHECK(Str::itoa<10>(123, temp, sizeof(temp)));
	CHECK(strcmp(temp, "123") == 0);

	CHECK(Str::itoa<10>(-345, temp, sizeof(temp)));
	CHECK(strcmp(temp, "-345") == 0);

	CHECK(Str::itoa<16>(24589, temp, sizeof(temp)));
	CHECK(strcmp(temp, "600d") == 0);

	CHECK(Str::itoa<16>(-24589, temp, sizeof(temp)));
	CHECK(strcmp(temp, "-600d") == 0);
}

TEST(Str, atoi) {
	int x;

	CHECK(Str::atoi<10>(x, "+1", 10));
	CHECK(x == 1);

	CHECK(Str::atoi<10>(x, "-1", 10));
	CHECK(x == -1);

	CHECK(Str::atoi<10>(x, "+123", 10));
	CHECK(x == 123);

	CHECK(Str::atoi<10>(x, "-123", 10));
	CHECK(x == -123);

	CHECK(!Str::atoi<10>(x, "-frob", 10));
}
