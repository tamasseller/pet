curdir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

INCLUDE_DIRS := $(INCLUDE_DIRS) $(curdir)/.

SOURCES := $(SOURCES) $(curdir)/data/PrettyPrinter.cpp

ifeq ($(PET_NO_1TEST),)
SOURCES := $(SOURCES) $(curdir)/1test/TestRunner.cpp
SOURCES := $(SOURCES) $(curdir)/1test/TraceOutput.cpp
SOURCES := $(SOURCES) $(curdir)/1test/PrintfOutput.cpp
SOURCES := $(SOURCES) $(curdir)/1test/FailureInjector.cpp
endif

undefine curdir