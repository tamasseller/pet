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

#ifndef MACRO_H_
#define MACRO_H_

#define INTERNAL_STRINGIFY_2(x)                 #x
#define INTERNAL_STRINGIFY(x)                   INTERNAL_STRINGIFY_2(x)
#define INTERNAL_AT()                           __FILE__ ":" INTERNAL_STRINGIFY(__LINE__)

#define ARG_NUMBER_TAGGER(F, _1, _2, _3, real, ...) F##real
#define VAR_ARG_MACRO(F, ...) ARG_NUMBER_TAGGER(F, ##__VA_ARGS__, 3, 2, 1, 0)(__VA_ARGS__)

#endif /* MACRO_H_ */
