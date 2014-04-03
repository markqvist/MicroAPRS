# specify compiler flags
CFLAGS = -I $(INC) $(CDEFS) -g -O0 -Werror -Wall

# set library name
LIBNAME  = libnmeap.a

COBJ     = nmeap01.o

INCLUDES=	$(INC)/nmeap.h $(INC)/nmeap_def.h

# build everything
all : $(LIB)/$(LIBNAME)

# build the library
$(LIB)/$(LIBNAME) : $(COBJ)
	-$(RM) $(LIB)/$(LIBNAME)
	$(AR)  -q $(LIB)/$(LIBNAME) $(COBJ)

# build all c files into .o files
$(COBJ): %.o: %.c
	$(CC) -c $(CFLAGS) $(SRC)/$< -o $@	

# erase all intermediate and output files
clean :
	-$(RM) *.o
	-$(RM) *~
	-$(RM) $(LIB)/$(LIBNAME)

# include file dependencies
$(COBJ) : $(INCLUDES)

