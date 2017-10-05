/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

#ifndef FAILUREINJECTOR_H_
#define FAILUREINJECTOR_H_

namespace pet {

class FailureInjector {
    static int max, counter, failAt;
    static bool enabled;
protected:
    friend class TestRunner;
    static void reset() {
        max = counter = failAt = 0;
        enabled = true;
    }

    static void firstRunDone() {
        max = counter;
    }

    static bool hasMore() {
        return failAt < max;
    }

    static bool step() {
        counter = 0;
        failAt++;
        return true;
    }
public:
    static bool shouldSimulateError()
    {
    	if(!enabled)
    		return false;
        counter++;
        return counter == failAt;
    }

    static void enable() {
    	enabled = true;
    }

    static void disable() {
    	enabled = false;
    }
};

}

#endif /* FAILUREINJECTOR_H_ */
