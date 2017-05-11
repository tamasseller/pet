OUTPUT = pet-test

# Library sources

SOURCES += ../1test/TestRunner.cpp
SOURCES += ../1test/TraceOutput.cpp
SOURCES += ../1test/FailureInjector.cpp

SOURCES += ../data/AvlTree.cpp
SOURCES += ../data/BinaryTree.cpp
SOURCES += ../ubiquitous/PrintfWriter.cpp

# Test code

#SOURCES += TestAlgoInline.cpp

SOURCES += TestAlgoIterate.cpp
SOURCES += TestAlgoStr.cpp
SOURCES += TestDataFifo.cpp
SOURCES += TestDataBinaryTree.cpp
SOURCES += TestDataDoubleList.cpp
SOURCES += TestDataLinkedList.cpp
SOURCES += TestHeapAvlStress.cpp
SOURCES += TestHeapBestFitStress.cpp
SOURCES += TestHeapHost1.cpp
SOURCES += TestHeapHost2.cpp
SOURCES += TestHeapHost3.cpp
SOURCES += TestHeapHost4.cpp
SOURCES += TestHeapHost5.cpp
SOURCES += TestHeapHost6.cpp
SOURCES += TestHeapTlsfStress.cpp
SOURCES += TestHeapTlsf.cpp
SOURCES += TestManagedHstring.cpp
SOURCES += TestManagedRegistry.cpp
SOURCES += TestManagedTreeMap.cpp
SOURCES += TestMetaExpLog.cpp
SOURCES += TestMetaExpRange.cpp
SOURCES += TestMetaFieldAccessor.cpp
SOURCES += TestMetaPacks.cpp
SOURCES += TestMetaRange.cpp
SOURCES += TestPoolArena.cpp
SOURCES += TestPoolFifo.cpp
SOURCES += TestPoolPool.cpp
SOURCES += TestPoolStack.cpp
SOURCES += TestPoolStaticStack.cpp
SOURCES += TestPoolStringStorage.cpp
SOURCES += TestUbiqFailable.cpp
SOURCES += TestUbiqError.cpp
SOURCES += TestUbiqTrace.cpp

# Test support

SOURCES += MockWriter.cpp
SOURCES += MockAllocator.cpp
SOURCES += DebugConfig.cpp
SOURCES += TestMain.cpp

# Includes

INCLUDE_DIRS += .
INCLUDE_DIRS += ..
INCLUDE_DIRS += failure-injector

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

include ultimate-makefile/Makefile.ultimate	
