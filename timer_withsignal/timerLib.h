#ifndef __TIMER_LIB_H__ 
#define __TIMER_LIB_H__ 

typedef int TIMERID; 

struct TIMER_EVENT { 
	struct TIMER_EVENT *prev, *next; 
	TIMERID id; 
	char name[64]; 
	int live; 
	pthread_mutex_t mutex; 
	pthread_cond_t cond; 
	long timevalue; 
	void (*func)(void *args); 
}; 

/* Add a timer with name, timevalue(uSeconds) and handler */ 
TIMERID add_timer(char *name, long timevalue, void *func); 

/* Delete a timer with id */ 
void del_timer(TIMERID id); 

#endif /* __TIMER_LIB_H__ */ 



