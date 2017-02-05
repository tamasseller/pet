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

#ifndef FAILABLECOMMON_H_
#define FAILABLECOMMON_H_


/* Example usage of custom error reporter for Failable.
	#include "CppUTest/TestHarness.h"
	#include "Backtrace.h"
	UNCHECKED_ERROR_REPORT() {
		Backtrace().print();
		FAIL("Unchecked Error");
	}
*/


/** @cond */

namespace ubiq {

class Global;

template<class Dummy> struct ErrorReporter;

class Police {
	struct ErrorHandle {
		bool good;
		unsigned int counter;
		inline ErrorHandle();
	};

	ErrorHandle *errorHandle;
public:
	inline void inspect();
	inline void cooperates();
	inline ~Police();
	inline Police();
	inline Police(const Police& old);
};

}

/** @endcond */


#define UNCHECKED_ERROR_REPORT()											\
namespace ubiq { template<> struct ErrorReporter<Global>: public Police {	\
	inline static void reportError();										\
};}																			\
																			\
inline void ubiq::ErrorReporter<ubiq::Global>::reportError()



#endif /* FAILABLECOMMON_H_ */
