/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

#ifndef PET_DATA_EXTREMESETFILTER_H_
#define PET_DATA_EXTREMESETFILTER_H_

#include "algorithm/MinMaxHeap.h"
#include "meta/Utility.h"

namespace pet {

template<class T, size_t n, bool (compare)(const T& a,const T& b)>
class ExtremeSetFilter: IndexedPrioQueueBase<ExtremeSetFilter<T, n, compare>>
{
	friend IndexedPrioQueueBase<ExtremeSetFilter<T, n, compare>> ;
	friend PrioQueueBase<ExtremeSetFilter<T, n, compare>, size_t>;

	/// Number of the already processed elements.
	size_t size = 0;

	/// Pointer to the buffer that is being sorted.
	T data[n];

	/// Inverse comparator, needed due to order reversal of extraction.
	inline bool compareElement(size_t a, size_t b) {
		return !compare(this->data[a], this->data[b]);
	}

	/// Implementation of the IndexedPrioQueueBase requirements.
	inline size_t getIndexLimit() {
		return size;
	}

	/// Implementation of the IndexedPrioQueueBase requirements.
	inline void swapElement(size_t a, size_t b)
	{
		const T temp = move(data[a]);
		data[a] = move(data[b]);
		data[b]= move(temp);
	}

public:
	template<class V>
	inline void insert(V&& v)
	{
		if(size < n)
		{
			data[size] = pet::forward<T>(v);
			this->heapUp(size);
			size++;
		}
		else if(compare(v, this->data[0]))
		{
			data[0] = pet::forward<T>(v);
			this->heapDown(0);
		}
	}

	inline void sort()
	{
		const auto s = size;

		while(size)
		{
	        swapElement(0, --size);
	        this->refresh(0);
		}

		size = s;
		for(auto i = 0; i < size / 2; i++)
	        swapElement(i, size - i - 1);
	}

	inline bool pop(T& v)
	{
		if(!size)
			return false;

		v = move(data[--size]);
		return true;
	}

	inline const T& extreme() {
		return data[size - 1];
	}

	inline bool isEmpty() {
		return size == 0;
	}
};

}

#endif /* PET_DATA_EXTREMESETFILTER_H_ */
