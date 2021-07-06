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

#include "RttIo.h"
#include "algorithm/Str.h"

#include "TraceCommon.h"

namespace pet {

/**
 * Trace writer that uses Segger RTT output.
 *
 * This writer is intended to be used in an embedded environment, where a
 * debugger that can process the RTT data flow is present.
 *
 * It writes the output directly to the circular buffer used for communication.
 */
template<class Rtt = RttIo<1024, 64>>
class RttWriter
{
	static constexpr bool disableInterruptsForCompleteLine = false;

public:
	inline RttWriter(pet::LogLevel, const char* name)
	{
		if constexpr(disableInterruptsForCompleteLine) { asm volatile ("cpsid i"); }
		if(name) { *this << name << ' '; }
	}

	inline ~RttWriter()
	{
		*this << '\n';
		if constexpr(disableInterruptsForCompleteLine) { asm volatile ("cpsie i"); }
	}

	/** Writes a character value to the output */
	inline RttWriter& operator<<(char val)
	{
		if constexpr(!disableInterruptsForCompleteLine) { asm volatile("cpsid i" ::: "memory"); }

		if(!Rtt::write(&val, 1))
		{
			asm volatile("bkpt 42" ::: "memory");
		}

		if constexpr(!disableInterruptsForCompleteLine) { asm volatile("cpsie i" ::: "memory"); }
		return *this;
	}

	/** Writes a zero terminated string to the output */
	inline RttWriter& operator<<(const char* val)
	{
		if constexpr(!disableInterruptsForCompleteLine) { asm volatile("cpsid i" ::: "memory"); }

		if(!Rtt::write(val, pet::Str::nLength(val, Rtt::upBufferLength)))
		{
			asm volatile("bkpt 42" ::: "memory");
		}

		if constexpr(!disableInterruptsForCompleteLine) { asm volatile("cpsie i" ::: "memory"); }
		return *this;
	}

	/** Writes a character value to the output */
	really_inline RttWriter& operator<<(signed char val) { return *this << (char)val; }

	/** Writes a character value to the output */
	really_inline RttWriter& operator<<(unsigned char val) { return *this << (char)val; }

	/** Writes a long value to the output in decimal */
	really_inline RttWriter& operator<<(long val) { return *this << ((int)val); }

	/** Writes an unsigned long value to the output in decimal */
	really_inline RttWriter& operator<<(unsigned long val) { return *this << ((unsigned int)val); }

	/** Writes an int value to the output in decimal */
	inline RttWriter& operator<<(int val)
	{
		char temp[16];
		pet::Str::itoa<10>(val, temp, sizeof(temp));
		return *this << (temp);
	}

	/** Writes an unsigned int value to the output in decimal */
	inline RttWriter& operator<<(unsigned int val)
	{
		char temp[16];
		pet::Str::utoa<10>(val, temp, sizeof(temp));
		return *this << (temp);
	}

	/** Writes a short value to the output in decimal */
	really_inline RttWriter& operator<<(short val) { return *this << ((int)val); }

	/** Writes an unsigned short value to the output in decimal */
	really_inline RttWriter& operator<<(unsigned short val) { return *this << ((unsigned int)val); }

	/** Writes a pointer value to the output in hexadecimal*/
	inline RttWriter& operator<<(const void* val)
	{
		char temp[16];
		pet::Str::utoa<16>((uintptr_t)val, temp, sizeof(temp));
		return *this << (temp);
	}

	inline RttWriter(RttWriter&&) = default;
	inline RttWriter(const RttWriter&) = default;
	inline RttWriter& operator =(RttWriter&&) = default;
	inline RttWriter& operator =(const RttWriter&) = default;
};

}

#endif /* PET_UBIQUITOUS_RTTWRITER_H_ */
