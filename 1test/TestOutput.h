/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <stdint.h>

namespace pet {

class TestOutput
{
	friend class TestRunner;
	virtual void reportProgress() {}
	virtual void reportTestFailure(bool isSynthetic, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text) = 0;
	virtual void reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic) = 0;
	inline virtual void reportProgress(const char* testName, bool isSynthetic) { this->reportProgress(); }

protected:
	virtual inline ~TestOutput() {}
};

}

#endif /* OUTPUT_H_ */
