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

#ifndef FNV_H_
#define FNV_H_

namespace algorithm {

/**
 * FNV hash.
 *
 * An implementation of the Fowler-Noll-Vo hash function.
 */
class Fnv {
	static const unsigned int fnvOffset = 2166136261;
	static const unsigned int fnvPrime = 16777619;
public:
	/**
	 * Hash of a *null-terminated* string.
	 *
	 * Iterates over a zero terminated character string, and calculates the FNV hash value for it.
	 * Does not include the terminator in the calculation.
	 *
	 * @param	str The string to be hashed.
	 * @return	The hash of the string.
	 */
	static inline unsigned int hash(const char* str){
		unsigned int hashres = fnvOffset;
		for(const char *idx = str; *idx; idx++){
			hashres ^= *idx;
			hashres *= fnvPrime;
		}
		return hashres;
	}

	/**
	 * Hash of a *non null-terminated* string.
	 *
	 * Iterates over a character string, until reaches the provided _end_ pointer,
	 * and calculates the FNV hash value for it.
	 * Does not include the character at the _end_ location.
	 *
	 * @param	str The string to be hashed.
	 * @param	end The limiting _end_ pointer.
	 * @return	The hash of the string.
	 *
	 */
	static inline unsigned int hash(const char* str, const char* end){
		unsigned int hashres = fnvOffset;
		for(const char *idx = str; idx != end; idx++){
			hashres ^= *idx;
			hashres *= fnvPrime;
		}
		return hashres;
	}
};

}

#endif /* FNV_H_ */
