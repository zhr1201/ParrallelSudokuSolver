
all:

EXTRA_CXXFLAGS = -Wno-sign-compare
include ../sudoku.mk

BINFILES = solve-sudoku solve-sudoku-mpi

OBJFILES =

TESTFILES =

MPITESTFILES =

ADDLIBS = ../solver/sudoku-solver.a ../util/sudoku-util.a ../generator/sudoku-generator.a 

include ../makefiles/default_rules.mk
