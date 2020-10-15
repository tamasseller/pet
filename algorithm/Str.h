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

#ifndef STR_H_
#define STR_H_

#define restrict __restrict

#include "meta/ExpLog.h"
#include "meta/Range.h"

#include <limits.h>

namespace pet {

/**
 * String utility functions.
 *
 * This class class provides static methods that implement basic string handling.
 * There some that are exactly the same as the ones of the standard C library,
 * others fill in the gaps that the stdlibc could not.
 */
struct Str {
	/**
	 * Same as std::strcmp.
	 *
	 * Compares two null-terminated strings.
	 *
	 * @param	s1 The first string.
	 * @param	s2 The second string.
	 * @return 	An integer less than, equal to, or greater than zero if s1 (or the first n bytes thereof)
	 * 			is found, respectively, to be less than, to match, or be greater than s2.

	 */
	static inline int cmp(const char * restrict s1, const char * restrict s2){
		while((*s1!='\0') || (*s2!='\0')) {
			if(*s1 > *s2)
				return 1;
			if(*s1 < *s2)
				return -1;
			s1++;
			s2++;
		}

		return 0;
	}

	/**
	 * Same as std::strncmp.
	 *
	 * Similar to strcmp, except it compares only some of the first bytes of the strings.
	 *
	 * @param 	s1 The first string.
	 * @param 	s2 The second string.
	 * @param 	len The length.
	 * @return 	An integer less than, equal to, or greater than zero if s1 (or the first n bytes thereof)
	 * 			is found, respectively, to be less than, to match, or be greater than s2.
	 */
	static inline int ncmp(const char * restrict s1, const char * restrict s2, unsigned int len){
		while(len && ((*s1!='\0') || (*s2!='\0'))) {
			if(*s1 > *s2)
				return 1;
			if(*s1 < *s2)
				return -1;
			s1++;
			s2++;
			len--;
		}

		return 0;
	}

	/**
	 * Same as std::strcpy.
	 *
	 * Copies a character string including the null terminator.
	 *
	 * @param 	out the destination.
	 * @param	in the source.
	 */
	static inline void cpy(char * restrict out, const char * restrict in){
		do{
			*out++ = *in++;
		}while(*in);
		*out = '\0';
	}

	/**
	 * Like std::strncpy, but *wihtout termination*.
	 *
	 * Copies a certain amount of characters from a string and does not *terminate* the destination.
	 *
	 * @param out the destination.
	 * @param in the source.
	 * @param len the length to be copied.
	 */
	static inline char * ntcpy(char * restrict out, const char * restrict in, unsigned int len){
		do{
			if(!len)
				break;

			len--;

			*out++ = *in++;
		}while(*in);
		return out;
	}

	/**
	 * Same as std::strncpy.
	 *
	 * Copies a certain amount of characters from a string and *terminates* the destination.
	 *
	 * @param 	out the destination.
	 * @param	in the source.
	 * @param	len	the length to be copied, not including the termination of the destination.
	 *
 	 * @note 	This behavior is useful when a string is stored in a fixed size area
	 * 			with the contract that either it is zero terminated or uses the whole
	 *			space available. This way the last byte is not wasted as terminator.
	 */
	static inline void ncpy(char * restrict out, const char * restrict in, unsigned int len){
		char* ret = ntcpy(out, in, len);
		*ret = '\0';
	}

	/**
	 * Like std::strlen, but with maximized length.
	 *
	 * Iterates over the character string and counts the initial non zero characters.
	 * Stops when encounters a zero, or reaches the specified maximal length.
	 * It *does not access* any characters *beyond the the limit*.
	 *
	 * @param 	str the character string.
	 * @param	len the maximal allowed length.
	 *
	 * @note 	This behavior is useful when a string is stored in a fixed size area
	 * 			with the contract that either it is zero terminated or uses the whole
	 *			space available. This way the last byte is not wasted as terminator.
	 */
	static inline unsigned int nLength(const char *str, unsigned int len)
	{
		unsigned int ret = 0;

		while(ret < len && str[ret] != '\0')
			ret++;

		return ret;
	}

	template<unsigned int base>
	static inline bool utoa(unsigned int x, char* output, unsigned int length)
	{
		static constexpr const char digits[] = "0123456789abcdef";

		static_assert(base <= (sizeof(digits)/sizeof(digits[0])), "Radix too high");

		constexpr auto maxPlaces = pet::log<base>::template x<UINT_MAX>::value;
		constexpr auto &table = pet::applyOverRange<pet::exp<base>::template x, 1, maxPlaces>::value;
		constexpr auto * const end = table + sizeof(table)/sizeof(table[0]);

		auto* p = &table[0];
		while(x >= *p && p < end) p++;

		auto n = (unsigned int)(p - table);

		if(!length || n >= length - 1)
			return false; // too long

		output += n;
		output[1] = '\0';

		do {
			*output-- = digits[x % base];
			x /= base;
		} while(n--);

		return true;
	}

	template<unsigned int base>
	static inline bool atou(unsigned int &x, const char* input, unsigned int length)
	{
		static_assert(base <= 16, "Radix too high");

		x = 0;

		char c = *input++;

		if(!c)
			return false;

		while(true) {
			if('0' <= c && c <= '9')
				x += c - '0';
			else if('a' <= c && c <= 'f')
				x += c - 'a' + 10;
			else if('A' <= c && c <= 'F')
				x += c - 'A' + 10;
			else
				return false;

			if(!(c = *input++) || !--length)
				break;

			x *= base;
		}

		return true;
	}

	template<unsigned int base>
	static inline bool itoa(int x, char* output, unsigned int length)
	{
		if(x < 0) {
			*output++ = '-';
			length -= 1;
			x = -x;
		}

		return utoa<base>(x, output, length);
	}


	template<unsigned int base>
	static inline bool atoi(int &x, const char* input, unsigned int length)
	{
		bool invert = *input == '-';

		if(*input == '+' || *input == '-') {
			input++;
			length--;
		}

		unsigned int ret;
		if(!atou<base>(ret, input, length))
			return false;

		x = (invert ? -(int)ret : (int)ret);
		return true;
	}
};

}
#endif /* STR_H_ */
