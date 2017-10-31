.PHONY: all
all : libaaron.a aaron_test

libaaron.a : aaron_lib.h aaron_lib.c aaron_hashmap.o
	gcc -g -c aaron_lib.c -Wall -Werror -o aaron_lib.o
	ar rc libaaron.a aaron_lib.o aaron_hashmap.o
	ranlib libaaron.a

aaron_test : aaron_test.h aaron_test.c libaaron.a
	gcc -g aaron_test.c libaaron.a -Wall -Werror -o aaron_test

aaron_hashmap.o : aaron_hashmap.h aaron_hashmap.c
	gcc -g -fpic -c aaron_hashmap.c -Wall -Werror -o aaron_hashmap.o

aaron_vector.o : aaron_vector.h aaron_vector.c
	gcc -g -fpic -c aaron_vector.c -Wall -Werror -o aaron_vector.o	

.PHONY: clean
clean :
	rm -f aaron_test libaaron.a *.o *~

clear_tests : 
	rm -f testfile*