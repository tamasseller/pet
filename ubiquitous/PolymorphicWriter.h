/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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

#ifndef PET_UBIQUITOUS_POLYMORPHICWRITER_H_
#define PET_UBIQUITOUS_POLYMORPHICWRITER_H_

#include "meta/Utility.h"
#include "ubiquitous/TraceCommon.h"

namespace pet
{

class PolymorphicWriter
{
public:
	class Receiver
	{
		friend PolymorphicWriter;
		virtual void operator<<(const char* val) = 0;
		virtual void operator<<(short val) = 0;
		virtual void operator<<(unsigned short val) = 0;
		virtual void operator<<(int val) = 0;
		virtual void operator<<(unsigned int val) = 0;
		virtual void operator<<(long val) = 0;
		virtual void operator<<(unsigned long val) = 0;
		virtual void operator<<(float val) = 0;
		virtual void operator<<(double val) = 0;
		virtual void operator<<(long double val) = 0;
		virtual void operator<<(const void* val) = 0;
		virtual void operator<<(const LineEnding &val) = 0;
		virtual void operator<<(const CollectionStart &val) = 0;
		virtual void operator<<(const CollectionEnd &val) = 0;
		virtual void operator<<(const ElementSeparator &val) = 0;
		virtual void operator<<(const KeyValueSeparator&val) = 0;

	public:
		inline virtual ~Receiver() {}
	};

private:
	Receiver* const impl;

protected:
	PolymorphicWriter(const PolymorphicWriter&) = delete;
	PolymorphicWriter& operator=(const PolymorphicWriter&) = delete;
	PolymorphicWriter(PolymorphicWriter&&) = delete;
	PolymorphicWriter& operator=(PolymorphicWriter&&) = delete;

	inline PolymorphicWriter(Receiver* impl): impl(impl) {}
	inline ~PolymorphicWriter() { delete impl; }

public:
	inline PolymorphicWriter& operator<<(const char* val)              { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(short val)                    { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(unsigned short val)           { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(int val)                      { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(unsigned int val)             { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(long val)                     { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(unsigned long val)            { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(float val)                    { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(double val)                   { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(long double val)              { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(const void* val)              { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(const LineEnding &val)        { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(const CollectionStart &val)   { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(const CollectionEnd &val)     { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(const KeyValueSeparator &val) { *impl << val; return *this; }
	inline PolymorphicWriter& operator<<(const ElementSeparator &val)  { *impl << val; return *this; }

};

template<class... Wrapped>
class PolymorphicTraceWriterWrapper: public PolymorphicWriter::Receiver, public Wrapped...
{
	template<class C>
	inline void fwd(C&& c)
	{
		int dummy[] = {0, (this->Wrapped::operator<<(forward<C>(c)), void(), 0)... };
		static_cast<void>(dummy);
	}

	virtual void operator<<(const char* val) override              { fwd(val); }
	virtual void operator<<(short val) override                    { fwd(val); }
	virtual void operator<<(unsigned short val) override           { fwd(val); }
	virtual void operator<<(int val) override                      { fwd(val); }
	virtual void operator<<(unsigned int val) override             { fwd(val); }
	virtual void operator<<(long val) override                     { fwd(val); }
	virtual void operator<<(unsigned long val) override            { fwd(val); }
	virtual void operator<<(float val) override                    { fwd(val); }
	virtual void operator<<(double val) override                   { fwd(val); }
	virtual void operator<<(long double val) override              { fwd(val); }
	virtual void operator<<(const void* val) override              { fwd(val); }
	virtual void operator<<(const LineEnding &val) override        { fwd(val); }
	virtual void operator<<(const CollectionStart &val) override   { fwd(val); }
	virtual void operator<<(const CollectionEnd &val) override     { fwd(val); }
	virtual void operator<<(const KeyValueSeparator &val) override { fwd(val); }
	virtual void operator<<(const ElementSeparator &val) override  { fwd(val); }

public:
	virtual inline ~PolymorphicTraceWriterWrapper() = default;

	inline PolymorphicTraceWriterWrapper(pet::LogLevel l, const char* n): Wrapped(l, n)... {}
};

}

#endif /* PET_UBIQUITOUS_POLYMORPHICWRITER_H_ */
