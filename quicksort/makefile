#*******************************************************************************
 # Name        : makefile
 # Author      : Connor Haaf and Ming Lin
 # Date        : 6/5/21
 # Description : Quicksort program makdfile.
 # Pledge      : I pledge my Honor that I have abided by the Stevens Honor System.
 #******************************************************************************
sort: sort.o quicksort.o # define dependencies for executable [sort] 
	gcc sort.o quicksort.o -o sort 
sort.o: sort.c
	gcc -Wall -Werror -pedantic-errors -c sort.c
quicksort.o: quicksort.c quicksort.h
	gcc -Wall -Werror -pedantic-errors -c quicksort.c
clean:
	rm -f *.o sort sort.exe