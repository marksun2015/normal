/*
 * gcc -o threadcpu threadcpu.c -lpthread
 */

#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/sysinfo.h>
#include<unistd.h>
 
#define __USE_GNU
#include<sched.h>
#include<ctype.h>
#include<string.h>
#include<pthread.h>

#define THREAD_MAX_NUM 10  //一個cpu內的thread個數
 
int num=0;  //cpu中核数
void* threadFun(void* arg)  
{
         cpu_set_t mask;  
         cpu_set_t get;   
         int *a = (int *)arg; //arg為thread編號 ,設定為cpu編號
         printf("the a is:%d\n",*a); 
         CPU_ZERO(&mask);    
         CPU_SET(*a,&mask); //a為cpu編號 ,透過mask設定再哪個cpu run
         if (sched_setaffinity(0, sizeof(mask), &mask) == -1)  //set thread running in cpu
         {
                   printf("warning: could not set CPU affinity, continuing...\n");
         }
         while (1)
         {
                   CPU_ZERO(&get);
                   if (sched_getaffinity(0, sizeof(get), &get) == -1)
                   {
                            printf("warning: cound not get thread affinity, continuing...\n");
                   }
                   int i;
                   for (i = 0; i < num; i++)
                   {
                            if (CPU_ISSET(i, &get))
                            {
                                     printf("this thread %d is running processor : %d\n", i,i);
                            }
                   }
         }
 
         return NULL;
}
 
int main(int argc, char* argv[])
{
         num = sysconf(_SC_NPROCESSORS_CONF); 
         pthread_t thread[THREAD_MAX_NUM];
         printf("system has %i processor(s). \n", num);
         int tid[THREAD_MAX_NUM];
         int i;
         for(i=0;i<num;i++)
         {
                   tid[i] = i;  //thread編號
                   pthread_create(&thread[0],NULL,threadFun,(void*)&tid[i]);
         }
         for(i=0; i< num; i++)
         {
                   pthread_join(thread[i],NULL);//等待thread結束，CTRL+C結束
         }
         return 0;
}
