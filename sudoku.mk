# Toolchain configuration

CXX = g++
AR = ar
AS = as
RANLIB = ranlib

# Base configuration

EXTLIBINC =
EXTLIBS = 
EXTLIBLDFLAGS = 

# Darwin (macOS) configuration

CXXFLAGS = -std=c++11 -I.. -I$(EXTLIBINC) -O1 $(EXTRA_CXXFLAGS) \
           -Wall -Wno-sign-compare -Wno-unused-local-typedefs \
           -Wno-deprecated-declarations -Winit-self \
           -g # -O0 -DSUDOKU_PARANOID

# Compiler specific flags
COMPILER = $(shell $(CXX) -v 2>&1)

LDFLAGS = $(EXTRA_LDFLAGS) $(EXTLIBLDFLAGS) -g
LDLIBS = $(EXTRA_LDLIBS) -lm -ldl

# Environment configuration
