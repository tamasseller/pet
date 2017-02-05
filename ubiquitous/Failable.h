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

#ifndef FAILABLE_H_
#define FAILABLE_H_

#include "ubiquitous/ConfigHelper.h"

namespace ubiq {

/** @cond */

template<class Dummy>
struct ErrorReporter {
	inline void inspect() {}
	inline void cooperates() {}
	inline void outAndOver() {}
	inline static void reportError() {}
};

inline Police::ErrorHandle::ErrorHandle(): good(false), counter(1) {}
inline Police::Police(): errorHandle(new ErrorHandle) {}

inline void Police::inspect() {
	if(!errorHandle->good) {
		ErrorReporter<Global>::reportError();
		errorHandle->good = true;
	}
}

inline void Police::cooperates() {
	errorHandle->good = true;
}

inline Police::~Police() {
	if(!--errorHandle->counter) {
		inspect();
		delete errorHandle;
	}
}

inline Police::Police(const Police& old): errorHandle(old.errorHandle) {
	errorHandle->counter++;
}

/** @endcond */

/**
 * A return value that can represent an error.
 *
 * This class template enables versatile test scenarious related to error handling. Any method can
 * wrap its return value into a failable by specifying the wrapped type and the values that
 * represent an exceptional state. If the user of the method does not check the returned value
 * explicitly for an error condition before using it , then - in a test build - a failure is induced.
 *
 * It is implemented in such a way that if the DebugConfig.h does not specify an error handler it
 * gets optimized away completely, that is the resulting code will be about the same as if the
 * wrapped type would have been used directly.
 *
 * It works internally like a shared_ptr, when it is created it allocates a small structure, that
 * holds a reference count and another variable representing if it was checked. The Failable itself
 * holds a pointer to that object. Any time a Failable is copied, the pointer is copied over and
 * the reference count is incremented. When it is destroyed the reference count is decremented
 * and if it reaches zero, then the flag is checked. If the error has not been checked until this
 * point, the alarm is triggered.
 *
 * @note This class is supposed to be used through the typedef ubiq::Failable.
 *
 * @tparam	Value			The wrapped type.
 * @tparam	Specific		CRTP style client that has to inherit from this class and
 * 							supply a bool isError() non-static method that returns true
 * 							if the current value is an error code. The method has to be
 * 							accessible from the FailableBase.
 * @tparam	ErrorReporter 	Internal parameter used for hacks related optimizing out
 * 							the additional code and the pointer for the error checking
 * 							helper struct if there is no reporter.
 */
template <class Specific, class Value, class ErrorReporter>
class FailableBase: private ErrorReporter {
protected:

	/** The wrapped value. */
	Value value;

public:
	/**
	 * Construct from value.
	 *
	 * Can be called implicitly, so that no change is needed to the body of a
	 * method to introduce this class into an existing codebase.
	 */
	inline constexpr FailableBase(const Value &value): value(value) {}

	/**
	 * Convert to value.
	 *
	 * Can be called implicitly, so that no change is needed to the body of a
	 * method to introduce this class into an existing codebase.
	 *
	 * @note 	Signals the ErrorReporter if there was no call to _failed_
	 * 			prior to the conversion.
	 */
	inline operator Value() {
		ErrorReporter::inspect();
		return value;
	};

	/**
	 * Check exceptional return value.
	 *
	 * Checks wether this value represents an error. Also registers that the client
	 * checked the error.
	 *
	 * @note	This method is needed to be called before the value is accessed.
	 */
	inline bool failed() {
		ErrorReporter::cooperates();
		return static_cast<Specific*>(this)->isError();
	};

	/**
	 * Create an unchecked copy.
	 *
	 * This method can be used when a Failable valued method (A) calls another
	 * Failable valued method (B) and checks its return value for an error, if A
	 * wants to return the value returned by B.
	 */
	inline Specific rethrow() {
		return Specific(this->value);
	}
};

/**
 * Failable facade.
 *
 * Wrapps automatic error reporting optimize-out hacks.
 *
 * @note 	This class is supposed to be used instead of ubiq::Failable.
 *
 * @tparam	Value			The wrapped type.
 * @tparam	Specific		CRTP style client that has to inherit from this class and
 * 							supply a bool isError() non-static method that returns true
 * 							if the current value is an error code. The method has to be
 * 							accessible from the FailableBase.
 */
template <typename Specific, typename Value>
using Failable = FailableBase<Specific, Value, ErrorReporter<Global>>;

}

#endif /* FAILABLE_H_ */
