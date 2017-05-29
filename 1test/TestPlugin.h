/*
 * TestPlugin.h
 *
 *  Created on: 2017.05.29.
 *      Author: tooma
 */

#ifndef TESTPLUGIN_H_
#define TESTPLUGIN_H_

namespace pet {

class TestRunner;
template<class> class LinkedList;

class TestPlugin {
	friend LinkedList<TestPlugin>;
	TestPlugin *next;
private:
	friend TestRunner;
	virtual void beforeTest() = 0;
	virtual void afterTest() = 0;
public:
	inline virtual ~TestPlugin() {}
};

}

#endif /* TESTPLUGIN_H_ */
