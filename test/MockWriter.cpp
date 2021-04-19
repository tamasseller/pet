/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#include "ubiquitous/Trace.h"

#include "1test/Mock.h"

#include "ubiquitous/PrintfWriter.h"

static inline bool checkWriterMocked()
{
	if(pet::TestRunner::isFailing())
		writerMocked = writerRecords = false;

	return writerMocked;
}

static inline bool checkWriterRecording()
{
	if(pet::TestRunner::isFailing())
		writerMocked = writerRecords = false;

	return writerRecords;
}

inline MwMockBackend::MwMockBackend(pet::LogLevel l, const char*) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("Writer").withParam((int)l);
}

inline MwMockBackend::~MwMockBackend() {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("~Writer");
}

inline void MwMockBackend::operator<<(const char* val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<const char*>").withParam(val);
}

inline void MwMockBackend::operator<<(short val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<short>").withParam(val);
}

inline void MwMockBackend::operator<<(unsigned short val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<unsigned short>").withParam(val);
}

inline void MwMockBackend::operator<<(int val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<int>").withParam(val);
}

inline void MwMockBackend::operator<<(unsigned int val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<unsigned int>").withParam(val);
}

inline void MwMockBackend::operator<<(long val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<long>").withParam(val);
}

inline void MwMockBackend::operator<<(unsigned long val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<unsigned long>").withParam(val);
}

inline void MwMockBackend::operator<<(float val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<float>").withParam(val);
}

inline void MwMockBackend::operator<<(double val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<double>").withParam(val);
}

inline void MwMockBackend::operator<<(long double val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<long double>").withParam(val);
}

inline void MwMockBackend::operator<<(const void* val) {
	if(checkWriterMocked()) MOCK(MockWriter)::CALL("write<void*>").withParam(val);
}

inline void MwRecordingBackend::operator<<(const char* val) {
	if(checkWriterRecording()) recorded += val;
}

inline void MwRecordingBackend::operator<<(short val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(unsigned short val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(int val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(unsigned int val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(long val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(unsigned long val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(float val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(double val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(long double val) {
	if(checkWriterRecording()) recorded += std::to_string(val);
}

inline void MwRecordingBackend::operator<<(const void* val) {
	if(checkWriterRecording()) FAIL("Unimplemented");
}

inline pet::PolymorphicWriter::Receiver* MockWriter::createReceiver(pet::LogLevel l, const char* name)
{
	if(writerMocked)
	{
		return state.construct<pet::PolymorphicTraceWriterWrapper<MwMockBackend>>(l, name);
	}
	else if(writerRecords)
	{
		return state.construct<pet::PolymorphicTraceWriterWrapper<MwRecordingBackend>>(l, name);
	}

	return state.construct<pet::PolymorphicTraceWriterWrapper<MwPrintfBackend>>(l, name);
}

MockWriter::MockWriter(pet::LogLevel l, const char* name): PolymorphicWriter(createReceiver(l, name)) {}
