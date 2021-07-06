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

/****** OpenOCD Setup (through GDB command line) ******

monitor rtt setup 0x20000000 0x100000 real-time-xfer
monitor rtt start
monitor rtt server start 37773 0
define hook-load
printf "stopping RTT\n"
monitor rtt stop
end
define hookpost-stop
printf "(re-)starting RTT\n"
monitor rtt start
end

**************** Netcat output setup ******************

nc localhost 37773

******************************************************/

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
