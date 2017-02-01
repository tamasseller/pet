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

#ifndef TRACECONFIG_H_
#define TRACECONFIG_H_

#include "MockWriter.h"
#include "CppUTestExt/MockSupport.h"

UNCHECKED_ERROR_REPORT() {
	mock().actualCall("failedAssert");
}

TRACE_WRITER(MockWriter);
GLOBAL_TRACE_POLICY(Failure);
CLIENT_TRACE_POLICY(B, Information);
CLIENT_TRACE_POLICY_NS(ns, C, Warning);

#endif /* TRACECONFIG_H_ */
