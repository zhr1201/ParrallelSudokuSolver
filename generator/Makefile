all: generatorProgram parserProgram

generatorProgram:
	g++ -std=c++11 generator.cc generator-test.cc -o generator

parserProgram:
	g++ -std=c++11 file-parser.cc file-parser-test.cc -o file-parser

clean: rmGenerator rmParser

rmGenerator:
	rm -f generator

rmParser:
	rm -f file-parser

#include ../sudoku.mk

#TESTFILES = generator-test

#OBJFILES = file-parser.o generator.o sudoku-problem.o

#LIBNAME = sudoku-generator

#ADDLIBS =

#include ../makefiles/default_rules.mk
