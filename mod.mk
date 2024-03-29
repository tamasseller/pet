curdir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

INCLUDE_DIRS := $(INCLUDE_DIRS) $(curdir)/.

SOURCES := $(SOURCES) $(curdir)/data/PrettyPrinter.cpp
SOURCES := $(SOURCES) $(curdir)/data/SharedAtomicList.cpp

ifeq ($(PET_NO_1TEST),)
SOURCES := $(SOURCES) $(curdir)/1test/TestRunner.cpp
SOURCES := $(SOURCES) $(curdir)/1test/TraceOutput.cpp
SOURCES := $(SOURCES) $(curdir)/1test/TestInterface.cpp
ifeq ($(PET_NO_1TEST_PRINTF_OUTPUT),)
SOURCES := $(SOURCES) $(curdir)/1test/PrintfOutput.cpp
endif
SOURCES := $(SOURCES) $(curdir)/1test/FailureInjector.cpp
endif

undefine curdir