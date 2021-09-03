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

#ifndef TRACEIMPL_H_
#define TRACEIMPL_H_

#include "ubiquitous/ConfigHelper.h"

namespace pet {

/** @cond */

template<class Dummy>
class DefaultTracePolicy {
public:
	constexpr static LogLevel level = LogLevel::None;
};

template<class Tag>
class TracePolicy {
public:	
	constexpr static LogLevel level = DefaultTracePolicy<Global>::level;
};

struct DummyWriter
{
	inline DummyWriter(...) {}

	template<class C>
	inline auto &operator<<(C&&) { return *this; }
};

template<class Dummy> struct TraceWriter { using Writer = DummyWriter; };

inline constexpr const char* getTagText(...) { return nullptr; }
template<class C> inline constexpr auto getTagText(C&& c) -> decltype(c.name){ return c.name; }

template<bool, LogLevel, class Tag> struct TraceFilter;

template<LogLevel level, class Tag> struct TraceFilter<true, level, Tag>
{
	inline auto operator() () {
		return typename TraceWriter<Global>::Writer(level, getTagText(Tag()));
	}
};

template<LogLevel level, class Tag> struct TraceFilter<false, level, Tag>
{
	inline auto operator() () {
		return DummyWriter{};
	}
};

template<class Tag, LogLevel level>
struct TraceSource: TraceFilter<level >= TracePolicy<Tag>::level, level, Tag> {};

/** @endcond */

/**
 * Tagged trace utility.
 * 
 * This class enables the client code to contain stl iostream style output
 * statements that can be selectively enabled or disabled without modifying
 * the client code itself. _When disabled_ all the outputting related operations
 * get _optimized out_.
 */
template<class Tag>
struct Trace {
	/**
	 * Output stream for messages intended for debugging.
	 * 
	 * Output through this stream can be configured with the LogLevel::Debug level identifier.
	 */
	static TraceSource<Tag, LogLevel::Debug> dbg;

	/**
	 * Output stream for informations.
	 *
	 * Output through this stream can be configured with the LogLevel::Information level identifier.
	 */
	static TraceSource<Tag, LogLevel::Information> info;

	/**
	 * Output stream for possible errors.
	 * 
	 * Output through this stream can be configured with the LogLevel::Warning level identifier.
	 */
	static TraceSource<Tag, LogLevel::Warning> warn;
	
	/**
	 * Output stream for non-fatal errors.
	 * 
	 * Output through this stream can be configured with the LogLevel::Failure level identifier.
	 */
	static TraceSource<Tag, LogLevel::Failure> fail;
	
	/**
	 * Output stream for fatal errors.
	 * 
	 * Output through this stream can be configured with the LogLevel::Critical level identifier.
	 */
	static TraceSource<Tag, LogLevel::Critical> crit;

	/**
	 * Output message on unmet condition.
	 * 
	 * This can be used instead of the stdlibc assert macro from the header assert.h.
	 * The level on which the message is to be output can be configured.
	 * If not specified explicitly the level defaults to Level::Failure.
	 */
	template<LogLevel level=LogLevel::Failure>
	static inline void assertThat(bool cond, const char* msg = "unspecified")
	{
		if constexpr(level >= TracePolicy<Tag>::level)
		{
			if(!cond)
			{
				TraceSource<Tag, level>().operator()() << "Assertation failed: " << msg << endl;
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////

template<class Tag>
TraceSource<Tag, LogLevel::Debug> 			Trace<Tag>::dbg;

template<class Tag>
TraceSource<Tag, LogLevel::Information> 	Trace<Tag>::info;

template<class Tag>
TraceSource<Tag, LogLevel::Warning> 		Trace<Tag>::warn;

template<class Tag>
TraceSource<Tag, LogLevel::Failure> 		Trace<Tag>::fail;

template<class Tag>
TraceSource<Tag, LogLevel::Critical> 		Trace<Tag>::crit;

}

#endif /* TRACEIMPL_H_ */
