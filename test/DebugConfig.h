/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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

#ifndef DEBUGCONFIG_H_
#define DEBUGCONFIG_H_

#include "1test/Test.h"
#include "1test/Mock.h"

#include <string>

#include "ubiquitous/ConfigHelper.h"

extern bool assertMocked;
extern bool writerMocked;
extern bool writerRecords;
extern std::string recorded;

#include "MockWriter.h"

TRACE_WRITER(MockWriter);
GLOBAL_TRACE_POLICY(Failure);
CLIENT_TRACE_POLICY(B, All);
CLIENT_TRACE_POLICY_NS(ns, C, Warning);

CLIENT_TRACE_POLICY(TestTraceTag, All);

UNCHECKED_ERROR_REPORT() 
{
	if(assertMocked) 
	{
		MOCK()::CALL(uncheckedErrorReport);
	}
	else 
	{
	    FAIL("Unchecked Error");
	}
}

#endif /* DEBUGCONFIG_H_ */
