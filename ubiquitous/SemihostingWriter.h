/*******************************************************************************
 *
 * Copyright (c) 2018 Tamás Seller. All rights reserved.
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

#ifndef SEMIHOSTINGWRITER_H_
#define SEMIHOSTINGWRITER_H_

#include "TraceCommon.h"

#include <stddef.h>
#include <stdint.h>

namespace pet {

/**
 * Trace writer that uses ARM semihosting output.
 * 
 * This writer is intended to be used in a hosted an ARM based embedded
 * environment, where a debugger that can process the semihosting requests
 * is present.
 *
 * It does buffer the output to counteract the exceptionally poor performance
 * of semihosting output. The buffer is flushed when a newline is encountered
 * or if it is full.
 */

class SemihostingWriter {
	static constexpr auto bufferSize = 4 * 1024u;
	static constexpr bool flushOnNewline = false;
	static char buffer[bufferSize];

	/** Index of the next byte to be written in the buffer */
	static uint32_t idx;

public:
	/** Method that flushes the semihosting output buffer if not empty already. */
	static void flush();

protected:

	/** Writes a zero terminated string to the output */
	SemihostingWriter& operator<<(const char* val);
	
	/** Writes a short value to the output */
	SemihostingWriter& operator<<(short val);
	
	/** Writes an unsigned short value to the output */
	SemihostingWriter& operator<<(unsigned short val);
	
	/** Writes an int value to the output */
	SemihostingWriter& operator<<(int val);
	
	/** Writes an unsigned int value to the output */
	SemihostingWriter& operator<<(unsigned int val);
	
	/** Writes a long value to the output */
	SemihostingWriter& operator<<(long val);
	
	/** Writes an unsigned long value to the output */
	SemihostingWriter& operator<<(unsigned long val);
	
	/** Writes a float value to the output */
	SemihostingWriter& operator<<(float val);
	
	/** Writes a double value to the output */
	SemihostingWriter& operator<<(double val);
	
	/** Writes a long value to the output */
	SemihostingWriter& operator<<(long double val);
	
	/** Writes a pointer value to the output */
	SemihostingWriter& operator<<(const void* val);

	inline SemihostingWriter(pet::LogLevel, const char*) {}

	SemihostingWriter(SemihostingWriter&&) = default;
	SemihostingWriter(const SemihostingWriter&) = default;
	SemihostingWriter& operator =(SemihostingWriter&&) = default;
	SemihostingWriter& operator =(const SemihostingWriter&) = default;
};

}

#endif /* PRINTFWRITER_H_ */
