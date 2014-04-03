INCLUDES= ..\inc\nmeap.h ..\inc\nmeap_def.h
CSRC    = nmeap01.c
LIBNAME = ..\lib\libnmeap.lib

# build everything
all : test1.exe test2.exe test3.exe wingps.exe 

test1.exe : test1.c $(LIBNAME)
	cl /DNDEBUG /c /I..\inc test1.c
	link /OUT:test1.exe test1.obj $(LIBNAME) 
	
test2.exe : test2.c $(LIBNAME)
	cl /DNDEBUG /c /I..\inc test2.c
	link /OUT:test2.exe test2.obj $(LIBNAME)
	
test3.exe : test3.c $(LIBNAME)
	cl /DNDEBUG /c /I..\inc test3.c
	link /OUT:test3.exe test3.obj $(LIBNAME)
	
wingps.exe : wingps.c $(LIBNAME)
	cl /DNDEBUG /c /I..\inc wingps.c
	link /OUT:wingps.exe wingps.obj $(LIBNAME)

# erase all intermediate and output files
clean :
	-erase *.obj
	-erase *.exe


