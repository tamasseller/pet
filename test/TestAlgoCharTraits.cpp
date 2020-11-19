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

#include "1test/Test.h"

#include "algorithm/CharTraits.h"

using namespace pet;

TEST_GROUP(CharTraits)
{
	template<class P>
	void test(bool (*f)(char), P&& p)
	{
		char c = 0;
		do
		{
			bool expected = p(c);
			CHECK(f(c) == expected);
		}
		while(++c);
	}
};

TEST(CharTraits, WhiteSpace)
{
	test(&CharTraits::isSpace, [](char c)
	{
		return 	c == ' '
			||	c == '\t'
			||	c == '\r'
			||	c == '\n';
	});
}

TEST(CharTraits, Operator)
{
	test(&CharTraits::isOperator, [](char c)
	{
		return c == '+'
			|| c == '-'
			|| c == '*'
			|| c == '/'
			|| c == '%'
			|| c == '<'
			|| c == '>'
			|| c == '='
			|| c == '|'
			|| c == '&'
			|| c == '^'
			|| c == '~'
			|| c == '!';
	});
}

TEST(CharTraits, UpperCase)
{
	test(&CharTraits::isUpper, [](char c)
	{
		return 'A' <= c && c <= 'Z';
	});
}

TEST(CharTraits, LowerCase)
{
	test(&CharTraits::isLower, [](char c)
	{
		return 'a' <= c && c <= 'z';
	});
}

TEST(CharTraits, Alpha)
{
	test(&CharTraits::isAlpha, [](char c)
	{
		return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
	});
}

TEST(CharTraits, IdInitial)
{
	test(&CharTraits::isIdInitial, [](char c)
	{
		return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
	});
}

TEST(CharTraits, Digit)
{
	test(&CharTraits::isDigit, [](char c)
	{
		return '0' <= c && c <= '9';
	});
}

TEST(CharTraits, IdInternal)
{
	test(&CharTraits::isIdInternal, [](char c)
	{
		return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_' || ('0' <= c && c <= '9');
	});
}

TEST(CharTraits, Comma)
{
	test(&CharTraits::isComma, [](char c)
	{
		return c == ',';
	});
}

TEST(CharTraits, Dot)
{
	test(&CharTraits::isDot, [](char c)
	{
		return c == '.';
	});
}

TEST(CharTraits, DoubleQuote)
{
	test(&CharTraits::isDoubleQuote, [](char c)
	{
		return c == '"';
	});
}

TEST(CharTraits, SingleQuote)
{
	test(&CharTraits::isSingleQuote, [](char c)
	{
		return c == '\'';
	});
}

TEST(CharTraits, OpenParen)
{
	test(&CharTraits::isOpenParen, [](char c)
	{
		return c == '(';
	});
}

TEST(CharTraits, CloseParen)
{
	test(&CharTraits::isCloseParen, [](char c)
	{
		return c == ')';
	});
}

TEST(CharTraits, OpenCurly)
{
	test(&CharTraits::isOpenCurly, [](char c)
	{
		return c == '{';
	});
}

TEST(CharTraits, CloseCurly)
{
	test(&CharTraits::isCloseCurly, [](char c)
	{
		return c == '}';
	});
}

TEST(CharTraits, OpenBox)
{
	test(&CharTraits::isOpenBox, [](char c)
	{
		return c == '[';
	});
}

TEST(CharTraits, CloseBox)
{
	test(&CharTraits::isCloseBox, [](char c)
	{
		return c == ']';
	});
}

TEST(CharTraits, Backslah)
{
	test(&CharTraits::isBackslash, [](char c)
	{
		return c == '\\';
	});
}
