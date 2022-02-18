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

#ifndef PET_1TEST_PRINTFOUTPUT_H_
#define PET_1TEST_PRINTFOUTPUT_H_

#include "TestOutput.h"

namespace pet {

class PrintfOutput: public TestOutput {
protected:
    int nDots = 0;
    virtual void reportProgress() override;
    virtual void reportTestFailure(uint32_t rerunIdx, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text) override;
    virtual void reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic) override;
    virtual inline ~PrintfOutput() {}

public:
    static PrintfOutput instance;
};

}

#endif /* PET_1TEST_PRINTFOUTPUT_H_ */
