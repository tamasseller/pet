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
    GenericError r;

    r = uut.nextReadable(buff1);
    CHECK(!r.failed() && r == 0);
	CHECK(!uut.doneReading(buff1, 0).failed());

    r = uut.nextWritable(buff1);
    CHECK(!r.failed() && r == 16);
	CHECK(!uut.doneWriting(buff1, 8).failed());

    r = uut.nextReadable(buff2);
    CHECK(!r.failed() && r == 8);
	CHECK(!uut.doneReading(buff2, 8).failed());

    CHECK(buff2 == buff1);
}

TEST(StaticFifo, Fill)
{
	char* buff;
	GenericError r;

	r = uut.nextWritable(buff);
    CHECK(!r.failed() && r == 16);
    CHECK(!uut.doneWriting(buff, 16).failed());

    r = uut.nextWritable(buff);
    CHECK(!r.failed() && r == 0);
}

TEST(StaticFifo, FillThenConsume)
{
	char* buff;
    GenericError r;

    r = uut.nextWritable(buff);
    CHECK(!r.failed() && r == 16);
	CHECK(!uut.doneWriting(buff, 16).failed());

    r = uut.nextReadable(buff);
    CHECK(!r.failed() && r == 16);
	CHECK(!uut.doneReading(buff, 8).failed());

    r = uut.nextWritable(buff);
    CHECK(!r.failed() && r == 8);
}

TEST(StaticFifo, FillEmptyThenRefill)
{
	char* buff;
    GenericError r;

	r = uut.nextWritable(buff);
	CHECK(!r.failed() &&  r == 16);
	CHECK(!uut.doneWriting(buff, 16).failed());

	r = uut.nextReadable(buff);
    CHECK(!r.failed() && r == 16);
	CHECK(!uut.doneReading(buff, 16).failed());

	r = uut.nextWritable(buff);
	CHECK(!r.failed() && r == 16);
	CHECK(!uut.doneWriting(buff, 16).failed());
}

TEST(StaticFifo, Exercise)
{
	char* buff;

    GenericError r;

	for(int i=0; i<16; i++) {
		for(uint32_t writeSize = 7; writeSize;) {
		    r = uut.nextWritable(buff);
		    CHECK(!r.failed());
			const uint32_t space = r;

			uint32_t written = 0;
			for(int j=0; j < space && writeSize; j++) {
				buff[j] = writeSize--;
				written++;
			}

			CHECK(!uut.doneWriting(buff, written).failed());
		}

		for(uint32_t readSize = 7; readSize;) {
            r = uut.nextReadable(buff);
            CHECK(!r.failed());
            const uint32_t space = r;

			uint32_t read = 0;
			for(int j=0; j < space && readSize; j++) {
				CHECK(buff[j] == readSize--);
				read++;
			}

			CHECK(!uut.doneReading(buff, read).failed());
		}

        r = uut.nextReadable(buff);
        CHECK(!r.failed() && r == 0);
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

#include <iostream>

TEST(IndirectFifo, ExerciseFull) {
	static constexpr const uint32_t prime1 = 13, prime2 = 7;
	uint32_t writeCounter = 0;
	uint32_t readCounter = 0;
    GenericError r;

	for(uint32_t readAmount = prime2; readAmount; readAmount = (readAmount + prime2) % prime1) {
		char* buff;
		while(1) {
            r = uut->nextWritable(buff);
            CHECK(!r.failed());
            const uint32_t space = r;

            if(!space)
                break;

			for(int i=0; i < space; i++)
				buff[i] = writeCounter++;

			CHECK(!uut->doneWriting(buff, space).failed());
		}

		for(uint32_t readSize = readAmount; readSize;) {
            r = uut->nextReadable(buff);
            CHECK(!r.failed());
            const uint32_t space = r;

			uint32_t read = 0;
			for(int j=0; j < space && read < readSize; j++) {
				CHECK(buff[j] == readCounter++);
				read++;
			}

			readSize -= read;
			CHECK(!uut->doneReading(buff, read).failed());
		}
	}
}

TEST(IndirectFifo, ExerciseEmpty) {
	static constexpr const uint32_t prime1 = 13, prime2 = 7;
	uint32_t writeCounter = 0;
	uint32_t readCounter = 0;
    GenericError r;

	for(uint32_t witeAmount = prime2; witeAmount; witeAmount = (witeAmount + prime2) % prime1) {
		char* buff;
		for(uint32_t writeSize = witeAmount; writeSize;) {
            r = uut->nextWritable(buff);
            CHECK(!r.failed());
            const uint32_t space = r;

			uint32_t written = 0;
			for(int j=0; j < space && written < writeSize; j++) {
				buff[j] = writeCounter++;
				written++;
			}

			writeSize -= written;

			CHECK(!uut->doneWriting(buff, written).failed());
		}

		while(1) {
            r = uut->nextReadable(buff);
            CHECK(!r.failed());
            const uint32_t space = r;

            if(!space)
                break;

			for(int i=0; i < space; i++)
				CHECK(buff[i] == readCounter++);

			CHECK(!uut->doneReading(buff, space).failed());
		}
	}
}
