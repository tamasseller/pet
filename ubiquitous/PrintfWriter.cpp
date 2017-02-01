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

#include "PrintfWriter.h"

#include <cstdio>

namespace ubiq {

void PrintfWriter::write(const char* val) {
	printf("%s", val);
}

void PrintfWriter::write(const void* val) {
	printf("%X", (int)val);
}

void PrintfWriter::write(short val) {
	printf("%hd", val);
}

void PrintfWriter::write(unsigned short val) {
	printf("%hu", val);
}

void PrintfWriter::write(int val) {
	printf("%d", val);
}

void PrintfWriter::write(unsigned int val) {
	printf("%u", val);
}

void PrintfWriter::write(long val) {
	printf("%ld", val);
}

void PrintfWriter::write(unsigned long val) {
	printf("%ld", val);
}

void PrintfWriter::write(float val) {
	printf("%f", val);
}

void PrintfWriter::write(double val) {
	printf("%f", val);
}

void PrintfWriter::write(long double val) {
	printf("%Lf", val);
}

}