/*******************************************************************************
 *
 * Copyright (c) 2021 Tamás Seller. All rights reserved.
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

#include "algorithm/Cobs.h"

#include <string>
#include <sstream>

using namespace pet;

TEST_GROUP(CobsCodec)
{
	class BufferHolder
	{
		char obuffer[0x10000] = {0x5a}, *op = obuffer;
		const char *ip, *iend;
		bool erred = false;

	public:
		inline BufferHolder(const std::string& str):
			ip(str.c_str()), iend(ip + str.size()) {}

	protected:
		inline char* writeAccess()
		{
			if(pet::FailureInjector::shouldSimulateError())
				erred = true;

			return erred ? nullptr : op++;
		}

		inline bool readInput(char &c)
		{
			if(ip != iend)
			{
				c = *ip++;
				return true;
			}

			return false;
		}

	public:
		inline std::string str()
		{
			std::string ret{obuffer, op};
			op = obuffer;
			return ret;
		}

		bool hasErred() {
			return erred;
		}
	};

	class TestDecoder: public BufferHolder, public CobsDecoder<TestDecoder>
	{
		friend TestDecoder::CobsDecoder;
		using BufferHolder::BufferHolder;
		using BufferHolder::writeAccess;
		using BufferHolder::readInput;

	public:
		static std::string decode(const std::string& input)
		{
			TestDecoder enc(input);
			CHECK(enc.decodeBlock());
			return enc.str();
		}
	};

	class TestEncoder: public BufferHolder, public CobsEncoder<TestEncoder>
	{
		friend TestEncoder::CobsEncoder;
		using BufferHolder::writeAccess;
		using BufferHolder::readInput;

	public:
		using BufferHolder::BufferHolder;
		static std::string encode(const std::string& input)
		{
			TestEncoder enc(input);
			CHECK(enc.encodeBlock());
			return enc.str();
		}
	};

	void failureInjectionTest(const std::string &testVector)
	{
		TestEncoder enc(testVector);
		bool encodeResult = enc.encodeBlock();

		auto encoded = enc.str();
		TestDecoder dec(encoded);
		bool decodeResult = dec.decodeBlock();

		CHECK_ALWAYS(encodeResult == !enc.hasErred());

		if(encodeResult)
		{
			CHECK_ALWAYS(decodeResult == !dec.hasErred());

			if(decodeResult)
			{
				auto decoded = dec.str();
				CHECK_ALWAYS(decoded == testVector);
			}
		}
	}

	void test(const std::string &testVector)
	{
		auto encoded = TestEncoder::encode(testVector);
		CHECK((testVector.size() + 1) <= encoded.size());
		CHECK(encoded.size() <= testVector.size() + (testVector.size() + 253) / 254 + 1);

		auto decoded = TestDecoder::decode(encoded);

	#if 0
		std::cout << "PLAIN: ";

		for(char c: testVector)
			std::cout << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)(uint8_t)c << ' ';

		std::cout << std::endl;

		std::cout << "ENCOD: ";

		for(char c: encoded)
			std::cout << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)(uint8_t)c << ' ';

		std::cout << std::endl;

		std::cout << "DECOD: ";

		for(char c: decoded)
			std::cout << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)(uint8_t)c << ' ';

		std::cout << std::endl << std::endl;
	#endif

		CHECK(decoded == testVector);
	}

	template<size_t n>
	void test(const char (&testVector)[n]) {
		test({testVector, testVector + n - 1});
	}

	char series[258];
	std::string testVectors[5];

	TEST_SETUP()
	{
		uint8_t n = 0;
		for(char& c: series)
			c = n++;

		testVectors[0] = {series + 1, series + 255};
		testVectors[1] = {series + 0, series + 255};
		testVectors[2] = {series + 1, series + 256};
		testVectors[3] = {series + 2, series + 257};
		testVectors[4] = {series + 3, series + 258};
	}
};

TEST(CobsCodec, ShortNonZeroInitial)
{
	pet::FailureInjector::disable();

	test("\x11");
	test("\x11\x00");
	test("\x11\x00\x00");
	test("\x11\x00\x00\x00");
}

TEST(CobsCodec, ShortAllZero)
{
	pet::FailureInjector::disable();

	test("\x00");
	test("\x00\x00");
	test("\x00\x00\x00");
}

TEST(CobsCodec, ShortZeroInitial)
{
	pet::FailureInjector::disable();

	test("\x00\x11");
	test("\x00\x11\x00");
	test("\x00\x00\x11");
}

TEST(CobsCodec, ShortMisc)
{
	pet::FailureInjector::disable();

	test("\x11\x22\x00");
	test("\x11\x22\x00\x33");
	test("\x11\x22\x33\x44");
}

TEST(CobsCodec, CanonicalLongTestVectorsFromWikipedia)
{
	pet::FailureInjector::disable();

	for(const auto& tv: testVectors)
		test(tv);
}

TEST(CobsCodec, FailureInjection)
{
	failureInjectionTest({series + 0, series + 255});
	failureInjectionTest({series + 1, series + 256});
}

TEST(CobsCodec, CatenatedCanonicalTestVectors)
{
	pet::FailureInjector::disable();

	std::stringstream buff;

	for(const auto& tv: testVectors)
	{
		buff << TestEncoder::encode(tv);
	}

	TestDecoder dec(buff.str());
	for(const auto& tv: testVectors)
	{
		CHECK(dec.decodeBlock());
		CHECK(dec.str() == tv);
	}

	CHECK(!dec.decodeBlock());
}
