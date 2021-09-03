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

#ifndef TRACECOMMON_H_
#define TRACECOMMON_H_

#include "meta/Utility.h"

namespace pet {

/**
 * Log levels.
 * 
 * These can be used to identify the different levels of 
 * tracing information for filtering.
 */
enum class LogLevel: int {
	All,			//!< Use for filters to enable all levels.
	Debug, 			//!< Normally disabled level for debugging.
	Information,	//!< Generic information level, with no intention to report any kind of error.
	Warning,		//!< Possibly erroneous level.
	Failure,		//!< Definitely erroneous but not fatal level.
	Critical,		//!< Fatal error level.
	None,			//!< Use for filters to disable all levels.
};

template<class> class TracePolicy;
template<class> class DefaultTracePolicy;
template<class> class TraceWriter;

struct LineEnding {
	operator const char*() const {
		return "\n";
	}
};

static constexpr inline auto endl = LineEnding();

struct CollectionStart {
	operator const char*() const {
		return "\t[";
	}
};

static constexpr inline auto coll = CollectionStart();

struct CollectionEnd {
	operator const char*() const {
		return "]";
	}
};

static constexpr inline auto endc = CollectionEnd();

struct ElementSeparator {
	operator const char*() const {
		return ",\t";
	}
};

static constexpr inline auto sep = ElementSeparator();

struct KeyValueSeparator {
	operator const char*() const {
		return ": ";
	}
};

static constexpr inline auto val = KeyValueSeparator();

class Global;

#define TRACE_WRITER(X) 							\
namespace pet {										\
	template<> struct TraceWriter<Global> {			\
		using Writer = X;							\
	};												\
}

#define __TRACE_POLICY(X, Y, Z)						\
namespace pet {										\
	template<>										\
	struct Z<X> {									\
		constexpr static LogLevel level				\
			= LogLevel::Y;							\
	};												\
}

}

#define CLIENT_TRACE_POLICY(X, Y) 					\
class X;											\
__TRACE_POLICY(X, Y, TracePolicy)					\

#define CLIENT_TRACE_POLICY_NS(NS, X, Y) 			\
namespace NS {class X;}								\
__TRACE_POLICY(NS::X, Y, TracePolicy)

#define GLOBAL_TRACE_POLICY(Y) __TRACE_POLICY(Global, Y, DefaultTracePolicy)

#endif /* TRACECOMMON_H_ */
