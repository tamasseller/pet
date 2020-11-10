/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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

#ifndef MOCKWRITER_H_
#define MOCKWRITER_H_

#include "ubiquitous/PrintfWriter.h"

class MockWriter: pet::PrintfWriter {
protected:
	static void write(const char* val);
	static void write(short val);
	static void write(unsigned short val);
	static void write(int val);
	static void write(unsigned int val);
	static void write(long val);
	static void write(unsigned long val);
	static void write(float val);
	static void write(double val);
	static void write(long double val);
	static void write(void* val);
};

#endif /* MOCKWRITER_H_ */
