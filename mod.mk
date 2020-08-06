curdir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

INCLUDE_DIRS := $(INCLUDE_DIRS) $(curdir)/.

SOURCES := $(SOURCES) $(curdir)/ubiquitous/PrintfWriter.cpp
SOURCES := $(SOURCES) $(curdir)/1test/TestRunner.cpp
SOURCES := $(SOURCES) $(curdir)/1test/TraceOutput.cpp
SOURCES := $(SOURCES) $(curdir)/1test/FailureInjector.cpp

undefine curdir