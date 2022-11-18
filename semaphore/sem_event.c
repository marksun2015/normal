/*
 *  gcc -o sem_event sem_event.c -lpthread
 */

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
sem_t sem;

void* change_resource(void *res)
{
    while (1) {
        printf("pthread wait\n");
        sem_wait(&sem);
        printf("sem post\n");
    }
}

int main()
{
    int ret;
    pthread_t thread; 
 
    ret = sem_init(&sem, 0, 0);
    if (ret == -1) {
        printf("sem_init failed \n");
        return -1;
    }

    ret = pthread_create(&thread, NULL, change_resource, NULL);
    if (ret != 0) {
        printf("pthread_create failed \n");
        return -1;
    }

    while (1) {
        printf("main thread \n");
        sleep(1);
        sem_post(&sem);
    }
}
