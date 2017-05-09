##Timer library test 
CC = gcc 
C_FLAGS = -Wall -O2 
LD_FLAGS = -lpthread 

all: test 
test: timerLib.o test.o 
	$(CC) $^ $(LD_FLAGS) -o test 
timerLib.o: timerLib.c timerLib.h 
	$(CC) $(C_FLAGS) -c timerLib.c 
test.o: test.c timerLib.h 
	$(CC) $(C_FLAGS) -c test.c 

clean: 
	rm -f *.o test 
