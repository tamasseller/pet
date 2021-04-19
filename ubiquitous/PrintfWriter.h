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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef PRINTFWRITER_H_
#define PRINTFWRITER_H_

#include "ubiquitous/TraceCommon.h"

namespace pet {

/**
 * Trace writer that uses printf.
 * 
 * This writer is intended to be used in a hosted test environment, where
 * stdlibc io functions are present and are quite straightforward to use.
 */
struct PrintfWriter
{
	/** Writes a zero terminated string to the output */
	PrintfWriter& operator<<(const char* val);
	
	/** Writes a short value to the output */
	PrintfWriter& operator<<(short val);
	
	/** Writes an unsigned short value to the output */
	PrintfWriter& operator<<(unsigned short val);
	
	/** Writes an int value to the output */
	PrintfWriter& operator<<(int val);
	
	/** Writes an unsigned int value to the output */
	PrintfWriter& operator<<(unsigned int val);
	
	/** Writes a long value to the output */
	PrintfWriter& operator<<(long val);
	
	/** Writes an unsigned long value to the output */
	PrintfWriter& operator<<(unsigned long val);
	
	/** Writes a float value to the output */
	PrintfWriter& operator<<(float val);
	
	/** Writes a double value to the output */
	PrintfWriter& operator<<(double val);
	
	/** Writes a long value to the output */
	PrintfWriter& operator<<(long double val);
	
	/** Writes a pointer value to the output */
	PrintfWriter& operator<<(const void* val);

	PrintfWriter(pet::Level, const char* name);
	~PrintfWriter();

	PrintfWriter(PrintfWriter&&) = delete;
	PrintfWriter(const PrintfWriter&) = delete;
	PrintfWriter& operator =(PrintfWriter&&) = delete;
	PrintfWriter& operator =(const PrintfWriter&) = delete;
};

}

#endif /* PRINTFWRITER_H_ */
