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

namespace pet
{

class PolymorphicWriter
{
public:
	class Receiver
	{
		friend PolymorphicWriter;
		virtual void write(const char* val) = 0;
		virtual void write(short val) = 0;
		virtual void write(unsigned short val) = 0;
		virtual void write(int val) = 0;
		virtual void write(unsigned int val) = 0;
		virtual void write(long val) = 0;
		virtual void write(unsigned long val) = 0;
		virtual void write(float val) = 0;
		virtual void write(double val) = 0;
		virtual void write(long double val) = 0;
		virtual void write(const void* val) = 0;
		virtual void write(const LineEnding &val) = 0;
		virtual void write(const CollectionStart &val) = 0;
		virtual void write(const CollectionEnd &val) = 0;
		virtual void write(const ElementSeparator &val) = 0;
		virtual void write(const KeyValueSeparator&val) = 0;

	public:
		inline virtual ~Receiver() {}
	};

private:
	static inline Receiver* impl = nullptr;

protected:
	static inline void write(const char* val) 				{impl->write(val);}
	static inline void write(short val) 					{impl->write(val);}
	static inline void write(unsigned short val) 			{impl->write(val);}
	static inline void write(int val) 						{impl->write(val);}
	static inline void write(unsigned int val) 				{impl->write(val);}
	static inline void write(long val) 						{impl->write(val);}
	static inline void write(unsigned long val) 			{impl->write(val);}
	static inline void write(float val) 					{impl->write(val);}
	static inline void write(double val) 					{impl->write(val);}
	static inline void write(long double val) 				{impl->write(val);}
	static inline void write(const void* val) 				{impl->write(val);}
	static inline void write(const LineEnding &val) 		{impl->write(val);}
	static inline void write(const CollectionStart &val) 	{impl->write(val);}
	static inline void write(const CollectionEnd &val) 		{impl->write(val);}
	static inline void write(const KeyValueSeparator &val) 	{impl->write(val);}
	static inline void write(const ElementSeparator &val) 	{impl->write(val);}

public:
	static inline void setReceiver(Receiver* r) {impl = r;}
};

template<class... Wrapped>
class PolymorphicTraceWriterWrapper: public PolymorphicWriter::Receiver, public Wrapped...
{
	template<class C>
	inline void fwd(C&& c)
	{
		int dummy[] = {0, (this->Wrapped::write(forward<C>(c)), void(), 0)... };
		static_cast<void>(dummy);
	}

	virtual inline void write(const char* val) 				{fwd(val);}
	virtual inline void write(short val) 					{fwd(val);}
	virtual inline void write(unsigned short val)			{fwd(val);}
	virtual inline void write(int val) 						{fwd(val);}
	virtual inline void write(unsigned int val) 			{fwd(val);}
	virtual inline void write(long val) 					{fwd(val);}
	virtual inline void write(unsigned long val) 			{fwd(val);}
	virtual inline void write(float val) 					{fwd(val);}
	virtual inline void write(double val) 					{fwd(val);}
	virtual inline void write(long double val) 				{fwd(val);}
	virtual inline void write(const void* val) 				{fwd(val);}
	virtual inline void write(const LineEnding &val) 		{fwd(val);}
	virtual inline void write(const CollectionStart &val) 	{fwd(val);}
	virtual inline void write(const CollectionEnd &val) 	{fwd(val);}
	virtual inline void write(const KeyValueSeparator &val) {fwd(val);}
	virtual inline void write(const ElementSeparator &val) 	{fwd(val);}

public:
	virtual inline ~PolymorphicTraceWriterWrapper() {}
};

}

#endif /* PET_UBIQUITOUS_POLYMORPHICWRITER_H_ */
