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

#ifndef TRACEIMPL_H_
#define TRACEIMPL_H_

#include "ubiquitous/ConfigHelper.h"

namespace pet {

/** @cond */

template<class Dummy>
class DefaultTracePolicy {
public:
	constexpr static Level level = Level::None;
};

template<class Tag>
class TracePolicy {
public:	
	constexpr static Level level = DefaultTracePolicy<Global>::level;
};

template<class Dummy>
class TraceWriter {
public:
	template<class T>
	inline static void write(T val) {}
};

template<class Tag, Level level>
class TraceSource {
private:
public:
	template<class T>
	inline TraceSource& operator<< (T val) {
		if(level >= TracePolicy<Tag>::level)
			TraceWriter<Global>::write(val);

		return *this;
	}
};

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
	 * Output stream for informations.
	 * 
	 * Output through this stream can be configured with the Level::Information level identifier.
	 */
	static TraceSource<Tag, Level::Information> info;

	/**
	 * Output stream for possible errors.
	 * 
	 * Output through this stream can be configured with the Level::Warning level identifier.
	 */
	static TraceSource<Tag, Level::Warning> warn;
	
	/**
	 * Output stream for non-fatal errors.
	 * 
	 * Output through this stream can be configured with the Level::Failure level identifier.
	 */
	static TraceSource<Tag, Level::Failure> fail;
	
	/**
	 * Output stream for fatal errors.
	 * 
	 * Output through this stream can be configured with the Level::Critical level identifier.
	 */
	static TraceSource<Tag, Level::Critical> crit;

	/**
	 * Output message on unmet condition.
	 * 
	 * This can be used instead of the stdlibc assert macro from the header assert.h.
	 * The level on which the message is to be output can be configured.
	 * If not specified explicitly the level defaults to Level::Failure.
	 */
	template<Level level=Level::Failure>
	static inline void assert(bool cond, const char* msg = "unspecified") {
		if(level >= TracePolicy<Tag>::level) {
			if(!cond) {
				TraceWriter<Global>::write("Assertation failed: ");
				TraceWriter<Global>::write(msg);
				TraceWriter<Global>::write(endl);
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////

template<class Tag>
TraceSource<Tag, Level::Information> 	Trace<Tag>::info;

template<class Tag>
TraceSource<Tag, Level::Warning> 		Trace<Tag>::warn;

template<class Tag>
TraceSource<Tag, Level::Failure> 		Trace<Tag>::fail;

template<class Tag>
TraceSource<Tag, Level::Critical> 		Trace<Tag>::crit;

}

#endif /* TRACEIMPL_H_ */
