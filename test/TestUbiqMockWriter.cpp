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
 * but WITHOUT ANY WARRANTY {
	mock("MockWriter").actualCall("write<>").withIntParameter("val", val);
}
 without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#include "CppUTestExt/MockSupport.h"

#include "MockWriter.h"

void MockWriter::write(const char* val) {
	mock("MockWriter").actualCall("write<const char*>").withStringParameter("val", val);
}

void MockWriter::write(void* val) {
	mock("MockWriter").actualCall("write<void*>").withPointerParameter("val", val);
}

void MockWriter::write(short val) {
	mock("MockWriter").actualCall("write<short>").withIntParameter("val", val);
}

void MockWriter::write(unsigned short val) {
	mock("MockWriter").actualCall("write<unsigned short>").withIntParameter("val", val);
}

void MockWriter::write(int val) {
	mock("MockWriter").actualCall("write<int>").withIntParameter("val", val);
}

void MockWriter::write(unsigned int val) {
	mock("MockWriter").actualCall("write<unsigned int>").withIntParameter("val", val);
}

void MockWriter::write(long val) {
	mock("MockWriter").actualCall("write<long>").withIntParameter("val", val);
}

void MockWriter::write(unsigned long val) {
	mock("MockWriter").actualCall("write<unsigned long>").withIntParameter("val", val);
}

void MockWriter::write(float val) {
	mock("MockWriter").actualCall("write<float>").withDoubleParameter("val", val);
}

void MockWriter::write(double val) {
	mock("MockWriter").actualCall("write<double>").withDoubleParameter("val", val);
}

void MockWriter::write(long double val) {
	mock("MockWriter").actualCall("write<long double>").withDoubleParameter("val", val);
}
