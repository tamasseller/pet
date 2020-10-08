/*******************************************************************************
 *
 * Copyright (c) 2018 Seller Tamás. All rights reserved.
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

#include "SemihostingWriter.h"

#include "algorithm/Str.h"

namespace pet {
	char SemihostingWriter::buffer[SemihostingWriter::bufferSize];

	uint32_t SemihostingWriter::idx = 0;

	void SemihostingWriter::flush()
	{
		if(idx) {
			uint32_t msg[3];
			msg[0] = 2;
			msg[1] = (uint32_t)buffer;
			msg[2] = idx;
			asm("mov r0, #5;"
				"mov r1, %[msg];"
				"bkpt #0xab"
				: // no output
				: [msg] "r" (msg)
				: "r0", "r1", "memory");

			idx = 0;
		}
	}

	void SemihostingWriter::write(const char* val) {
		while(*val) {
			buffer[idx++] = *val;

			if(idx == sizeof(buffer) || ((*val == '\n') && flushOnNewline))
				flush();

			val++;
		}
	}

	void SemihostingWriter::write(long val) {
		write((int)val);
	}

	void SemihostingWriter::write(unsigned long val) {
		write((unsigned int)val);
	}

	void SemihostingWriter::write(int val) {
		char temp[16];
		pet::Str::itoa<10>(val, temp, sizeof(temp));
		write(temp);
	}

	void SemihostingWriter::write(unsigned int val) {
		char temp[16];
		pet::Str::utoa<10>(val, temp, sizeof(temp));
		write(temp);
	}

	void SemihostingWriter::write(short val) {
		write((int)val);
	}

	void SemihostingWriter::write(unsigned short val) {
		write((unsigned int)val);
	}

	void SemihostingWriter::write(const void* val) {
		write((uintptr_t)val);
	}
}
