# Project Name
TARGET = GoldenVoice

# Sources
CPP_SOURCES = GoldenVoice.cpp

# Library Locations
LIBDAISY_DIR = ../DAISY/DAISY_CORE/libdaisy
DAISYSP_DIR = ../DAISY/DAISY_CORE/DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

