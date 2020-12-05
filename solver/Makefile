all:


include ../sudoku.mk

TESTFILES = solver-test

MPITESTFILES = solver-mpi-test

OBJFILES = problem-state.o solver.o solver-omp.o solver-serial.o solver-mpi.o

LIBNAME = sudoku-solver

ADDLIBS = ../util/sudoku-util.a ../validator/sudoku-validator.a

include ../makefiles/default_rules.mk
