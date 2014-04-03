INCLUDES= ..\inc\nmeap.h ..\inc\nmeap_def.h
CSRC    = nmeap01.c
LIBNAME = ..\lib\libnmeap.lib

# build everything
all : $(LIBNAME)

$(LIBNAME) : nmeap01.obj
	-erase $(LIBNAME)
	lib /OUT:$(LIBNAME) nmeap01.obj

nmeap01.obj : nmeap01.c $(INCLUDES)
	cl /DNDEBUG /c /I..\inc nmeap01.c

# erase all intermediate and output files
clean :
	-erase *.obj
	-erase $(LIBNAME)


