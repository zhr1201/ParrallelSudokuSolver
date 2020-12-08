# This is the top-level Makefile.
# Also see sudoku.mk which supplies options and some rules
# used by the Makefiles in the subdirectories.

SHELL := /bin/bash

SUBDIRS = util generator generatorbin \
          solver solverbin validator validatorbin

SUBDIRS_LIB = $(filter-out %bin, $(SUBDIRS))

SUDOKU_SONAME ?= libsudoku.so

include sudoku.mk

# Reset the default goal, so that the all target will become default
.DEFAULT_GOAL :=
all: $(SUBDIRS)
	-echo Done

mklibdir:
	test -d $(SUDOKULIBDIR) || mkdir $(SUDOKULIBDIR)

# I don't want to call rm -rf
rmlibdir:
ifneq ($(SUDOKULIBDIR), )
	-rm $(SUDOKULIBDIR)/*{.so,.a,.o}
	-rmdir $(SUDOKULIBDIR)
else
	@true
endif

# Compile optional stuff
ext: $(SUBDIRS)
	-echo Done

clean: rmlibdir
	-for x in $(SUBDIRS); do $(MAKE) -C $$x clean; done

distclean: clean
	-for x in $(SUBDIRS); do $(MAKE) -C $$x distclean; done

test: $(addsuffix /test, $(SUBDIRS_LIB))

testmpi: $(addsuffix /mpitest, $(SUBDIRS_LIB))

# Define an implicit rule, expands to e.g.:
#  base/test: base
#     $(MAKE) -C base test
%/test: % mklibdir
	$(MAKE) -C $< test

%/mpitest: % mklibdir
	$(MAKE) -C $< mpitest

util/.depend.mk:
	$(MAKE) depend

depend: $(addsuffix /depend, $(SUBDIRS))

%/depend:
	$(MAKE) -C $(dir $@) depend

.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	$(MAKE) -C $@

### Dependency list ###
# this is necessary for correct parallel compilation
#1)The tools depend on all the libraries
evalbin generatorbin probleminfobin solverbin: \
 generator solver util validator

#2)The libraries have inter-dependencies (only use util for now, other dependencies to be added)
generator: util
probleminfo: util
validator: util
solver: util validator
