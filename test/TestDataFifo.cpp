/*******************************************************************************
 *
 * Copyright (c) 2017 Tamás Seller. All rights reserved.
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

#include "data/Fifo.h"

#include "CppUTest/TestHarness.h"

using namespace pet;

TEST_GROUP(StaticFifo) {
	typedef StaticFifo<16> Uut;
	Uut uut;
};

TEST(StaticFifo, Sanity)
{
	char* buff1, *buff2;
	CHECK(uut.isEmpty());
	CHECK(!uut.isFull());
	CHECK(!uut.nextReadable(buff1));
	uut.doneReading(0);

	CHECK(uut.nextWritable(buff1) == 16);
	uut.doneWriting(8);
	CHECK(!uut.isEmpty());
	CHECK(!uut.isFull());

	CHECK(uut.nextReadable(buff2) == 8);
	CHECK(buff2 == buff1);
	uut.doneReading(8);
	CHECK(uut.isEmpty());
	CHECK(!uut.isFull());
}

TEST(StaticFifo, Fill)
{
	char* buff;

	CHECK(uut.isEmpty());
	CHECK(!uut.isFull());

	CHECK(uut.nextWritable(buff) == 16);
	uut.doneWriting(16);

	CHECK(!uut.isEmpty());
	CHECK(uut.isFull());

	CHECK(!uut.nextWritable(buff));

	CHECK(!uut.isEmpty());
	CHECK(uut.isFull());
}

TEST(StaticFifo, FillThenConsume)
{
	char* buff;

	CHECK(uut.isEmpty());
	CHECK(!uut.isFull());

	CHECK(uut.nextWritable(buff) == 16);
	uut.doneWriting(16);

	CHECK(!uut.isEmpty());
	CHECK(uut.isFull());

	CHECK(uut.nextReadable(buff) == 16);
	uut.doneReading(8);

	CHECK(!uut.isEmpty());
	CHECK(!uut.isFull());

	CHECK(uut.nextWritable(buff) == 8);
	uut.doneReading(8);

	CHECK(uut.isEmpty());
	CHECK(!uut.isFull());
}

TEST(StaticFifo, FillEmptyThenRefill)
{
	char* buff;
	CHECK(uut.nextWritable(buff) == 16);
	uut.doneWriting(16);

	CHECK(uut.nextReadable(buff) == 16);
	uut.doneReading(16);

	CHECK(uut.nextWritable(buff) == 16);
	uut.doneWriting(16);
}

TEST(StaticFifo, Exercise)
{
	char* buff;

	for(int i=0; i<16; i++) {
		for(uint32_t writeSize = 7; writeSize;) {
			const uint32_t space = uut.nextWritable(buff);
			uint32_t written = 0;
			for(int j=0; j < space && writeSize; j++) {
				buff[j] = writeSize--;
				written++;
			}

			uut.doneWriting(written);
		}

		for(uint32_t readSize = 7; readSize;) {
			uint32_t space = uut.nextReadable(buff);
			uint32_t read = 0;
			for(int j=0; j < space && readSize; j++) {
				CHECK(buff[j] == readSize--);
				read++;
			}

			uut.doneReading(read);
		}

		CHECK(!uut.nextReadable(buff));
	}
}

TEST_GROUP(IndirectFifo) {
	typedef IndirectFifo<16> Uut;
	char *buffer;
	Uut *uut;

	TEST_SETUP() {
		uut = new Uut(buffer = new char[16]);
	}

	TEST_TEARDOWN() {
		delete uut;
		delete buffer;
	}
};

TEST(IndirectFifo, ExerciseFull) {
	static constexpr const uint32_t prime1 = 13, prime2 = 7;
	uint32_t writeCounter = 0;
	uint32_t readCounter = 0;
	for(uint32_t readAmount = prime2; readAmount; readAmount = (readAmount + prime2) % prime1) {
		char* buff;

		while(const uint32_t space = uut->nextWritable(buff)) {
			for(int i=0; i < space; i++)
				buff[i] = writeCounter++;

			uut->doneWriting(space);
		}

		for(uint32_t readSize = readAmount; readSize;) {
			uint32_t space = uut->nextReadable(buff);
			uint32_t read = 0;
			for(int j=0; j < space && read < readSize; j++) {
				CHECK(buff[j] == readCounter++);
				read++;
			}

			readSize -= read;
			uut->doneReading(read);
		}
	}
}

TEST(IndirectFifo, ExerciseEmpty) {
	static constexpr const uint32_t prime1 = 13, prime2 = 7;
	uint32_t writeCounter = 0;
	uint32_t readCounter = 0;
	for(uint32_t witeAmount = prime2; witeAmount; witeAmount = (witeAmount + prime2) % prime1) {
		char* buff;
		for(uint32_t writeSize = witeAmount; writeSize;) {
			uint32_t space = uut->nextWritable(buff);
			uint32_t written = 0;
			for(int j=0; j < space && written < writeSize; j++) {
				buff[j] = writeCounter++;
				written++;
			}

			writeSize -= written;

			uut->doneWriting(written);
		}

		while(const uint32_t space = uut->nextReadable(buff)) {
			for(int i=0; i < space; i++)
				CHECK(buff[i] == readCounter++);

			uut->doneReading(space);
		}
	}
}
