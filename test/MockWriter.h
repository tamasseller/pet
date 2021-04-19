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

#ifndef MOCKWRITER_H_
#define MOCKWRITER_H_

#include "ubiquitous/PrintfWriter.h"
#include "ubiquitous/PolymorphicWriter.h"
#include "data/MutableStorage.h"

struct MwMockBackend
{
	inline MwMockBackend(pet::LogLevel, const char*);
	inline ~MwMockBackend();
	inline void operator delete(void*) {}

	inline void operator<<(const char* val);
	inline void operator<<(short val);
	inline void operator<<(unsigned short val);
	inline void operator<<(int val);
	inline void operator<<(unsigned int val);
	inline void operator<<(long val);
	inline void operator<<(unsigned long val);
	inline void operator<<(float val);
	inline void operator<<(double val);
	inline void operator<<(long double val);
	inline void operator<<(const void* val);
};

struct MwRecordingBackend
{
	inline MwRecordingBackend(pet::LogLevel, const char*) {}
	inline void operator delete(void*) {}

	inline void operator<<(const char* val);
	inline void operator<<(short val);
	inline void operator<<(unsigned short val);
	inline void operator<<(int val);
	inline void operator<<(unsigned int val);
	inline void operator<<(long val);
	inline void operator<<(unsigned long val);
	inline void operator<<(float val);
	inline void operator<<(double val);
	inline void operator<<(long double val);
	inline void operator<<(const void* val);
};

struct MwPrintfBackend: pet::PrintfWriter
{
	using PrintfWriter::PrintfWriter;
	inline void operator delete(void*) {}
};

class MockWriter: public pet::PolymorphicWriter
{
	inline pet::PolymorphicWriter::Receiver* createReceiver(pet::LogLevel l, const char* name);

	pet::MutableStorage <
		pet::PolymorphicTraceWriterWrapper<MwMockBackend>,
		pet::PolymorphicTraceWriterWrapper<MwPrintfBackend>,
		pet::PolymorphicTraceWriterWrapper<MwRecordingBackend>
	> state;

public:
	MockWriter(pet::LogLevel, const char*);
	inline ~MockWriter() = default;

	MockWriter(MockWriter&&) = delete;
	MockWriter(const MockWriter&) = delete;
	MockWriter& operator =(MockWriter&&) = delete;;
	MockWriter& operator =(const MockWriter&) = delete;
};

#endif /* MOCKWRITER_H_ */
