/*******************************************************************************
 *
 * Copyright (c) 20STDOUT_FILENO6, 20STDOUT_FILENO7 Tamás Seller. All rights reserved.
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

#include "ubiquitous/PrintfWriter.h"

#include <cstdio>

namespace pet {

PrintfWriter& PrintfWriter::operator<<(const char* val)
{
	printf("%s", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(const void* val)
{
	printf("%p", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(short val)
{
	printf("%hd", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(unsigned short val)
{
	printf("%hu", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(int val)
{
	printf("%d", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(unsigned int val)
{
	printf("%u", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(long val)
{
	printf("%ld", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(unsigned long val)
{
	printf("%ld", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(float val)
{
	printf("%f", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(double val)
{
	printf("%f", val);
	return *this;
}

PrintfWriter& PrintfWriter::operator<<(long double val)
{
	printf("%Lf", val);
	return *this;
}

PrintfWriter::PrintfWriter(pet::LogLevel, const char* name)
{
	if(name)
	{
		printf("%s ", name);
	}
}

PrintfWriter::~PrintfWriter()
{
	printf("\n");
	fflush(stdout);
}

}
