OUTPUT = pet-meta-test

# Library sources

SOURCES += ../../1test/TestRunner.cpp
SOURCES += ../../1test/FailureInjector.cpp
# Test code

SOURCES += TestGroups.cpp
SOURCES += TestMock.cpp
SOURCES += TestNoGroup.cpp
SOURCES += TestFailureInjector.cpp

# Test support

SOURCES += TestMain.cpp

# Includes

INCLUDE_DIRS += .
INCLUDE_DIRS += ../..

# Flags

CXXFLAGS += -std=c++11								# Use the c++ standard released in 2011
CXXFLAGS += -O0 									# Avoid optimizer screwing up coverage data
CXXFLAGS += -g3										# Include all available debug info
CXXFLAGS += -fno-inline								# Do not inline (coverage again)
CXXFLAGS += -m32									# Use 32 bit mode (to model the target MCUs)
CXXFLAGS += -fmax-errors=5							# Avoid blowing the console
CXXFLAGS += -rdynamic								# Allows meaningful backrtaces 
CXXFLAGS += --coverage								# Generate program flow data and inject profiling code
CXXFLAGS += -fno-exceptions
CXXFLAGS += -fdelete-null-pointer-checks

#TEST_CMD_PREFIX=LD_LIBRARY_PATH=/opt/gcc-5.3.0/lib32/
#CXX=/opt/gcc-5.3.0/bin/g++
LD=$(CXX) 

include ../ultimate-makefile/Makefile.ultimate	
