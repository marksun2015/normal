#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>
#include "timerLib.h" 


void timer_handler1(void) 
{ 
    printf("Hello timer handle1...\n"); 
} 

void timer_handler2(void) 
{ 
	printf("Hello timer handle2...\n");
} 

int main(int argc, char *argv[]) 
{ 
	TIMERID id1, id2; 
	id1 = add_timer("testT1",  2000000, timer_handler1); 
	id2 = add_timer("testT2",  5000000, timer_handler2); 
	for (;;) { 
		switch(getchar()) 
		{ 
			case 'd': 
    			printf("delete handle1...\n"); 
				del_timer(id1); 
			break; 
			case 'D': 
    			printf("delete handle2...\n"); 
				del_timer(id2); 
			break; 
			default: 
			break; 
		} 
	} 
	return 0; 
} 


