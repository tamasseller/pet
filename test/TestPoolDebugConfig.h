/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#include "CppUTest/TestHarness.h"
#undef new

#include "Addr2lineBacktrace.h"
#include "PrintfWriter.h"

UNCHECKED_ERROR_REPORT() {
	Addr2lineBacktrace().print();
	FAIL("Unchecked Error");
}

TRACE_WRITER(PrintfWriter);
//CLIENT_TRACE_POLICY(MockAllocatorTrace, Information);

// GLOBAL_TRACE_POLICY(Failure);
// CLIENT_TRACE_POLICY(SomeClass, Information);
// CLIENT_TRACE_POLICY_NS(mm, AllHeapsTrace, Warning);

#endif /* DEBUGCONFIG_H_ */
