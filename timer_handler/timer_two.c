/*
 * gcc -o timer timer.c -lrt
 */
#include<stdio.h>
#include<signal.h>
#include<time.h>
#include<string.h>

void handle1(union sigval v)  
{  
    time_t t;  
    char p[32];  
  
    time(&t);  
    strftime(p, sizeof(p), "%T", localtime(&t));  
  
    printf("time is handle1:%d %s\n", v, p);  
}  
  
void handle2(union sigval v)  
{  
    printf("time is handle2:%d \n",v);  
}  
  
int main()  
{  
    struct sigevent evp1;  
    struct itimerspec ts1;  
    timer_t timerid1;  
    
    struct sigevent evp2;  
    struct itimerspec ts2;  
    timer_t timerid2; 
 
    int ret;  
  
    memset(&evp1, 0, sizeof(struct sigevent));
    memset(&evp2, 0, sizeof(struct sigevent));

    evp1.sigev_value.sival_int = 111;
    evp1.sigev_notify = SIGEV_THREAD; 
    evp1.sigev_notify_function = handle1;  

    evp2.sigev_value.sival_int = 112;
    evp2.sigev_notify = SIGEV_THREAD; 
    evp2.sigev_notify_function = handle2;  
    
    ret = timer_create(CLOCK_REALTIME, &evp1, &timerid1);  
    if( ret )   
        perror("timer_create");  
 
    ret = timer_create(CLOCK_REALTIME, &evp2, &timerid2);  
    if( ret )   
        perror("timer_create");  
 
    /////////////////////////
    ts1.it_interval.tv_sec = 1;  
    ts1.it_interval.tv_nsec = 0;  
    ts1.it_value.tv_sec = 1;  
    ts1.it_value.tv_nsec = 0;  
  
    ret = timer_settime(timerid1, 0, &ts1, NULL);  
    if( ret )   
        perror("timer_settime");  
  
    /////////////////////////
    ts2.it_interval.tv_sec = 1;  
    ts2.it_interval.tv_nsec = 0;  
    ts2.it_value.tv_sec = 1;  
    ts2.it_value.tv_nsec = 0;  
    
    ret = timer_settime(timerid2, 0, &ts2, NULL);  
    if( ret )   
        perror("timer_settime");  
    while(1);  
} 
