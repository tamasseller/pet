/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#ifndef FW_LIB_PET_DATA_LINREGFILTER_H_
#define FW_LIB_PET_DATA_LINREGFILTER_H_

#include <stdint.h>

template<size_t n, size_t shift>
struct LinRegFilter
{
	struct Data {
		int32_t x = 0, y = 0;
	} data[n];

	int32_t sumX = 0;
	int32_t sumY = 0;
	int32_t sumXsq = 0;
	int32_t sumXy = 0;
	size_t idx = 0;

	static inline int32_t mult(int32_t x, int32_t y) {
		return (x >> shift) * (y >> shift);
	}

	static inline int32_t sq(int32_t x) {
		return mult(x, x);
	}

	void update(int32_t x, int32_t y, int32_t &numA, int32_t &numB, int32_t &denom)
	{
		const int32_t oldX = data[idx].x;
		const int32_t oldY = data[idx].y;
		data[idx].x = x;
		data[idx].y = y;

		if(++idx >= n)
			idx = 0;

		sumX -= oldX;
		sumY -= oldY;
		sumXsq -= sq(oldX);
		sumXy -= mult(oldX, oldY);

		sumX += x;
		sumY += y;
		sumXsq += sq(x);
		sumXy += mult(x, y);

		denom = sq(sumX) - n * sumXsq;
		numA = mult(sumX, sumY) - n * sumXy;
		numB = mult(sumX, sumXy) - mult(sumXsq, sumY);
	}
};

#endif /* FW_LIB_PET_DATA_LINREGFILTER_H_ */
