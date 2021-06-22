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

#ifndef PET_UBIQUITOUS_RTTWRITER_CPP_
#define PET_UBIQUITOUS_RTTWRITER_CPP_

#include "RttWriter.h"

#include "meta/Sequence.h"
#include "platform/Compiler.h"
#include "algorithm/Str.h"

#include <stdint.h>
#include <stddef.h>

using namespace pet;

char RttWriter::buffer[RttWriter::bufferSize];

/*

OpenOCD Setup (through GDB command line)

set $rttcbaddr=&pet::RttWriter::controlBlock
set $rttcbsize=sizeof(pet::RttWriter::controlBlock)
eval "monitor rtt setup 0x%x 0x%x rtt-trace", $rttcbaddr, $rttcbsize
monitor rtt start
monitor rtt server start 37773 0
define hook-load
monitor rtt stop
end
define hookpost-load
monitor rtt start
end


Netcat output

nc localhost 37773

 */

RttWriter::RttWriter(pet::LogLevel, const char* name)
{
	if constexpr(disableInterruptsForCompleteLine)
	{
		asm volatile ("cpsid i");
	}

	if(name)
	{
		*this << name << ' ';
	}
}

RttWriter::~RttWriter()
{
	*this << '\n';

	if constexpr(disableInterruptsForCompleteLine)
	{
		asm volatile ("cpsie i");
	}
}

struct RttWriter::UpBufferDescriptor
{
	const char* const name = "Terminal";
	char* const data = buffer;
	const uint32_t size = bufferSize;
	volatile uint32_t wIdx = 0, rIdx = 0;
	uint32_t flag = 0;

	inline void write(const char* str, size_t length)
	{
		if constexpr(!disableInterruptsForCompleteLine)
		{
			asm volatile("cpsid i" ::: "memory");
		}

		const auto used = wIdx - rIdx;
		const auto free = sizeof(buffer) - 1 - used;

		if(unlikely(length > free))
		{
			asm volatile("bkpt 42" ::: "memory");
		}
		else
		{
			const auto end = wIdx + length;

			if(end < sizeof(buffer))
			{
				__builtin_memcpy(buffer + wIdx, str, length);
				wIdx = end;
			}
			else
			{
				const auto leftover = end - sizeof(buffer);
				__builtin_memcpy(buffer + wIdx, str, length - leftover);
				__builtin_memcpy(buffer, str, leftover);
				wIdx = leftover;
			}
		}

		if constexpr(!disableInterruptsForCompleteLine)
		{
			asm volatile ("cpsie i" ::: "memory");
		}
	}
};

template<class> struct InvertedString;

template<int... idx>
class InvertedString<pet::Sequence<idx...>>
{
public:
	static constexpr auto length = sizeof...(idx);
	constexpr inline InvertedString(const char (&in)[length]): value{(char)~in[idx]...} {}
	really_inline constexpr char operator[](size_t n) const { return value[n]; }

private:
	const char value[length];
};

struct alignas(256) RttWriter::ControlBlock
{
	static constexpr inline auto obfuscatedId = InvertedString<pet::sequence<0, sizeof(RttWriter::id)>>(RttWriter::id);

	volatile char id[16];
	int nUpBuffers = 1;
	int nDownBuffers = 0;
	UpBufferDescriptor terminal;

	inline ControlBlock()
	{
		for(auto i = 0u; i < sizeof(id); i++)
		{
			id[i] = i < obfuscatedId.length ? ~obfuscatedId[i] : 0;
		}
	}

	inline ~ControlBlock()
	{
		for(auto &c: id)
		{
			c = 0;
		}
	}
};

RttWriter::ControlBlock RttWriter::controlBlock;


RttWriter& RttWriter::operator<<(char val)
{
	controlBlock.terminal.write(&val, 1);
	return *this;
}

RttWriter& RttWriter::operator<<(signed char val) {
	return *this << (char)val;
}

RttWriter& RttWriter::operator<<(unsigned char val) {
	return *this << (char)val;
}

RttWriter& RttWriter::operator<<(const char* val)
{
	controlBlock.terminal.write(val, pet::Str::nLength(val, sizeof(buffer)));
	return *this;
}

RttWriter& RttWriter::operator<<(long val) {
	return *this << ((int)val);
}

RttWriter& RttWriter::operator<<(unsigned long val) {
	return *this << ((unsigned int)val);
}

RttWriter& RttWriter::operator<<(int val)
{
	char temp[16];
	pet::Str::itoa<10>(val, temp, sizeof(temp));
	return *this << (temp);
}

RttWriter& RttWriter::operator<<(unsigned int val)
{
	char temp[16];
	pet::Str::utoa<10>(val, temp, sizeof(temp));
	return *this << (temp);
}

RttWriter& RttWriter::operator<<(short val) {
	return *this << ((int)val);
}

RttWriter& RttWriter::operator<<(unsigned short val) {
	return *this << ((unsigned int)val);
}

RttWriter& RttWriter::operator<<(const void* val)
{
	char temp[16];
	pet::Str::utoa<16>((uintptr_t)val, temp, sizeof(temp));
	return *this << (temp);
}

#endif /* PET_UBIQUITOUS_RTTWRITER_CPP_ */
