/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

void MockWriter::write(const char* val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<const char*>").withParam(val);
	else if(writerRecords)
		recorded += val;
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(void* val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<void*>").withParam(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(short val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<short>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(unsigned short val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<unsigned short>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(int val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<int>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(unsigned int val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<unsigned int>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(long val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<long>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(unsigned long val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<unsigned long>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(float val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<float>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(double val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<double>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}

void MockWriter::write(long double val)
{
	if(writerMocked)
		MOCK(MockWriter)::CALL("write<long double>").withParam(val);
	else if(writerRecords)
		recorded += std::to_string(val);
	else
		pet::PrintfWriter::write(val);
}
