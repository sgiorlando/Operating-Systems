FLAGS= -Wall -Wextra -g

all: main.o datastruct.o buddysys.o
	gcc -o main main.o datastruct.o buddysys.o $(FLAGS)

main.o: buddysys.h datastruct.h main.c
	gcc -c main.c $(FLAGS)

buddysys.o: datastruct.h buddySystem.h buddy.c
	gcc -c buddy.c $(FLAGS)

datastruct.o: datastruct.h datastruct.c
	gcc -c datastruct.c $(FLAGS)

clean:
	rm -rf *~ *.o main
