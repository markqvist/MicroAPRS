all : test1 test2 test3 

test1 : $(LIB)/libnmeap.a $(TST)/test1.c 
	gcc -g -O0 -I $(INC) $(CDEFS) -Wall -Werror -o test1 $(TST)/test1.c $(LIB)/libnmeap.a

test2 : $(LIB)/libnmeap.a $(TST)/test2.c 
	gcc -g -O0 -I $(INC) $(CDEFS) -Wall -Werror -o test2 $(TST)/test2.c $(LIB)/libnmeap.a

test3 : $(LIB)/libnmeap.a $(TST)/test3.c 
	gcc -g -O0 -I $(INC) $(CDEFS) -Wall -Werror -o test3 $(TST)/test3.c $(LIB)/libnmeap.a

clean:
	-$(RM) test1
	-$(RM) test2
	-$(RM) test3


