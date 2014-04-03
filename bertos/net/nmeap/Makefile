# rules
export CC=gcc
export CDEFS = -DNDEBUG

# directories
BASE :=$(shell pwd)
export SRC=$(BASE)/src
export TST=$(BASE)/tst
export INC=$(BASE)/inc
export LIB=$(BASE)/lib

all :
	cd $(SRC) && $(MAKE) all
	cd $(TST) && $(MAKE) all
	

clean :
	cd $(SRC) && $(MAKE) clean
	cd $(TST) && $(MAKE) clean
	
doc  :
	doxygen

