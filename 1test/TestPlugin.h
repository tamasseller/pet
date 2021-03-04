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

#ifndef TESTPLUGIN_H_
#define TESTPLUGIN_H_

namespace pet {

class TestRunner;
template<class> class LinkedPtrList;

class TestPlugin
{
	friend LinkedPtrList<TestPlugin*>;
	TestPlugin *next;

	friend TestRunner;
	virtual void beforeTest() = 0;
	virtual void afterTest(bool failed) = 0;
public:

	inline virtual ~TestPlugin() {}
};

}

#endif /* TESTPLUGIN_H_ */
