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

#include "RttIo.h"

#include <cstring>

/****************************/
/*                          */
/*      OpenOCD config      */
/*                          */
/****************************/

/*

$_TARGETNAME configure -event gdb-flash-write-start {

    rtt stop
}

$_TARGETNAME configure -event semihosting-user-cmd-0x100 {
	set command {rtt }
	set params [arm semihosting_read_user_param]
	eval "$command $params" 
}

init
arm semihosting on

*/

using namespace pet;

bool RttBufferDescriptor::write(const char* str, size_t length)
{
	auto wIt = startWriting();

	const char* const end = str + length;

	for(auto rIt = str; rIt != end; rIt++)
	{
		if(auto ptr = writeAccess(wIt))
		{
			*ptr = *rIt;
		}
		else
		{
			return false;
		}
	}

	commitWrite(wIt);
	return true;
}


static void sendTclCommand(const char* const command, const uint32_t len)
{
	uint32_t msg[2];
	msg[0] = (uint32_t)command;
	msg[1] = len;

	asm
	(
		"ldr r0, =0x100;"
		"mov r1, %[msg];"
		"bkpt #0xab"
		: // no output
		: [msg] "r" (msg)
		: "r0", "r1", "memory"
	);
}

template<size_t n>
static inline void sendTclCommand(const char (&command)[n]) {
	sendTclCommand(command, n);
}

static inline void writeHex(unsigned int x, char* output)
{
	static constexpr const char digits[] = "0123456789abcdef";

	for(unsigned int i = 0; i < 32; i += 4)
	{
		const auto nibble = (x << i) >> 28;
		*output++ = digits[nibble];
	}
}

void RttControlBlock::setupOpenocdRttSink()
{
    sendTclCommand("stop");

    char setupCommand[] = "setup 0x________ 0x________                  auto_stop";
    writeHex((uint32_t)this, setupCommand + 8);
    writeHex(sizeof(*this), setupCommand + 19);
    memcpy(setupCommand + 28, (const char*)id, obfuscatedId.length);

    sendTclCommand(setupCommand);
    sendTclCommand("server start pipe 0");
    sendTclCommand("start");
}

RttControlBlock::~RttControlBlock()
{
	for(auto &c: id)
	{
		c = 0;
	}
}
