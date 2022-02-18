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

#ifndef CHARTRAITS_H_
#define CHARTRAITS_H_

#include "algorithm/CharLookupTable.h"

namespace pet
{

struct CharTraits
{
    static inline bool isSpace(char c) {
        return CharLookupTable<' ', '\t', '\r', '\n'>::contains(c);
    }

    static inline bool isOperator(char c) {
        return CharLookupTable<'+', '-', '*', '/', '%', '<', '>', '=', '|', '&', '^', '~', '!'>::contains(c);
    }

    static inline bool isUpper(char c) {
        return CharLookupTable<'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'>::contains(c);
    }

    static inline bool isLower(char c) {
        return CharLookupTable<'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'>::contains(c);
    }

    static inline bool isAlpha(char c) {
        return CharLookupTable<'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                               'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'>::contains(c);
    }

    static inline bool isIdInitial(char c) {
        return CharLookupTable<'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                               'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                               '_'>::contains(c);
    }

    static inline bool isDigit(char c) {
        return CharLookupTable<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'>::contains(c);
    }

    static inline bool isHexDigit(char c) {
        return CharLookupTable<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F'>::contains(c);
    }

    static inline bool isIdInternal(char c) {
        return CharLookupTable<'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                               'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                               '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                               '_'>::contains(c);
    }

    static inline bool isComma(char c) {
        return c == ',';
    }

    static inline bool isDot(char c) {
        return c == '.';
    }

    static inline bool isDoubleQuote(char c) {
        return c == '"';
    }

    static inline bool isSingleQuote(char c) {
        return c == '\'';
    }

    static inline bool isOpenParen(char c) {
        return c == '(';
    }

    static inline bool isCloseParen(char c) {
        return c == ')';
    }

    static inline bool isOpenCurly(char c) {
        return c == '{';
    }

    static inline bool isCloseCurly(char c) {
        return c == '}';
    }

    static inline bool isOpenBox(char c) {
        return c == '[';
    }

    static inline bool isCloseBox(char c) {
        return c == ']';
    }

    static inline bool isBackslash(char c) {
        return c == '\\';
    }
};

}

#endif /* CHARTRAITS_H_ */
