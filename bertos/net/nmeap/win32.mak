# directories
SRC=src
TST=tst

all :
	cd $(SRC)
	$(MAKE) -f nmeap.mak all
	cd ..\$(TST) 
	$(MAKE) -f tst.mak all
	cd ..
	

clean :
	cd $(SRC)
	$(MAKE) -f nmeap.mak clean
	cd ..\$(TST)
	$(MAKE) -f tst.mak clean
	cd ..
	
doc  :
	doxygen

