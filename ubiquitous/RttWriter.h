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

#ifndef PET_UBIQUITOUS_RTTWRITER_H_
#define PET_UBIQUITOUS_RTTWRITER_H_

#include "TraceCommon.h"

#include <stddef.h>
#include <stdint.h>

namespace pet {

/**
 * Trace writer that uses Segger RTT output.
 *
 * This writer is intended to be used in an embedded environment, where a
 * debugger that can process the semihosting requests is present.
 *
 * It writes the output directly to the circular buffer used for communication.
 */
class RttWriter
{
	static constexpr bool disableInterruptsForCompleteLine = false;
	static constexpr auto bufferSize = 4 * 1024u;
	static char buffer[bufferSize];

	struct UpBufferDescriptor;
	struct ControlBlock;
	static ControlBlock controlBlock;

public:
	static constexpr char id[] = "rtt-trace";

	/** Writes a zero terminated string to the output */
	RttWriter& operator<<(const char* val);

	/** Writes a character value to the output */
	RttWriter& operator<<(char val);
	RttWriter& operator<<(signed char val);
	RttWriter& operator<<(unsigned char val);

	/** Writes a short value to the output */
	RttWriter& operator<<(short val);

	/** Writes an unsigned short value to the output */
	RttWriter& operator<<(unsigned short val);

	/** Writes an int value to the output */
	RttWriter& operator<<(int val);

	/** Writes an unsigned int value to the output */
	RttWriter& operator<<(unsigned int val);

	/** Writes a long value to the output */
	RttWriter& operator<<(long val);

	/** Writes an unsigned long value to the output */
	RttWriter& operator<<(unsigned long val);

	/** Writes a float value to the output */
	RttWriter& operator<<(float val);

	/** Writes a double value to the output */
	RttWriter& operator<<(double val);

	/** Writes a long value to the output */
	RttWriter& operator<<(long double val);

	/** Writes a pointer value to the output */
	RttWriter& operator<<(const void* val);

	RttWriter(pet::LogLevel, const char* name);
	~RttWriter();

	RttWriter(RttWriter&&) = default;
	RttWriter(const RttWriter&) = default;
	RttWriter& operator =(RttWriter&&) = default;
	RttWriter& operator =(const RttWriter&) = default;
};

}

#endif /* PET_UBIQUITOUS_RTTWRITER_H_ */
