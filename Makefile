# This is the top-level Makefile.
# Also see sudoku.mk which supplies options and some rules
# used by the Makefiles in the subdirectories.

SHELL := /bin/bash

SUBDIRS = util eval evalbin generator generatorbin \
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

# Define an implicit rule, expands to e.g.:
#  base/test: base
#     $(MAKE) -C base test
%/test: % mklibdir
	$(MAKE) -C $< test

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
 eval generator probleminfo solver util

#2)The libraries have inter-dependencies (only use util for now, other dependencies to be added)
eval: util 
generator: util
probleminfo: util
solver: util validator
