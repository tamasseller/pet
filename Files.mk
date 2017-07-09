#
# This file can be used to decouple internal build structure of this 
# library from that of the user's. It can be used in a build system
# that depends on relative paths (for example the ultimate-makfile)
# like this:
#
# include ../pet/Files.mk
# SOURCES += $(shell realpath --relative-to . $(PET_SOURCES))
#
# Assuming that the SOURCES makefile variable holds the paths of the
# source files that are to be built into the final product.
#

__PET_FILES_MK_OWN_DIR = $(abspath $(dir $(filter %Files.mk, $(MAKEFILE_LIST))))

PET_SOURCES += $(__PET_FILES_MK_OWN_DIR)/ubiquitous/PrintfWriter.cpp
PET_SOURCES += $(__PET_FILES_MK_OWN_DIR)/data/BinaryTree.cpp
PET_SOURCES += $(__PET_FILES_MK_OWN_DIR)/data/AvlTree.cpp
PET_SOURCES += $(__PET_FILES_MK_OWN_DIR)/1test/TraceOutput.cpp
PET_SOURCES += $(__PET_FILES_MK_OWN_DIR)/1test/FailureInjector.cpp
PET_SOURCES += $(__PET_FILES_MK_OWN_DIR)/1test/TestRunner.cpp

