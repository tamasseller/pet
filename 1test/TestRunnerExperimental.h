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

#ifndef _TESTRUNNEREXPERIMENTAL_H_
#define _TESTRUNNEREXPERIMENTAL_H_

#include "TestRunner.h"

namespace pet {

class TestRunner::Experimental 
{
    static void timerHandler(int);
    static int runTestSubset(const char* filter, int timeLimitSec);
    
public:
    static int runTestsInParallel(int timeLimitSec = 30, const char* filter = nullptr);

    static int main(int argc, const char* argv[], TestOutput* output = &TraceOutput::instance);
};

}

#endif /* _TESTRUNNEREXPERIMENTAL_H_ */
