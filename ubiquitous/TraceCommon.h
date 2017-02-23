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

#ifndef TRACECOMMON_H_
#define TRACECOMMON_H_

namespace pet {

/**
 * Log levels.
 * 
 * These can be used to identify the different levels of 
 * tracing information for filtering.
 */
enum class Level: int {
	All,			//!< Use for filters to enable all levels.
	Information,	//!< Generic information level, with no intention to report any kind of error.
	Warning,		//!< Possibly erroneus level.
	Failure,		//!< Definitely erroneus but not fatal level.
	Critical,		//!< Fatal error level.
	None,			//!< Use for filters to disable all levels.
};

/** @cond */

class Global;

template<class>
class TracePolicy;

template<class>
class DefaultTracePolicy;

template<class>
class TraceWriter;

#define TRACE_WRITER(X) 							\
namespace pet {										\
	template<> struct TraceWriter<Global>: X {		\
		using X::write;								\
	};												\
}


#define __TRACE_POLICY(X, Y, Z)						\
namespace pet {										\
	template<>										\
	struct Z<X> {									\
		constexpr static Level level = Level::Y;	\
	};												\
}

}

#define CLIENT_TRACE_POLICY(X, Y) 					\
class X;											\
__TRACE_POLICY(X, Y, TracePolicy);					\

#define CLIENT_TRACE_POLICY_NS(NS, X, Y) 			\
namespace NS {class X;}								\
__TRACE_POLICY(NS::X, Y, TracePolicy);

#define GLOBAL_TRACE_POLICY(Y) __TRACE_POLICY(Global, Y, DefaultTracePolicy);

/** @endcond */

#endif /* TRACECOMMON_H_ */
