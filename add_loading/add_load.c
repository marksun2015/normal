/*
 * 程式說明：
 * 作者：ianwolf 日期：2010/2/3
 * 目的：增加CPU想要的負載值
 * 使用方法：第一個參數為想要增加的負載值(0~100) 第二個參數是取樣load的取樣時間間隔(單位：ms)
 * 執行例子： ./add_load 50 20
 * 代表讓CPU增加50的負載，取樣間格為20ms
 * 編譯方法：gcc -D_REENTRANT add_load.c -o add_load -lpthread
 * */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define msleep(x) usleep(x*1000)//函式庫只有usleep而沒有msleep，所以用此種方法取代
pthread_mutex_t exec_mutex;//mutex變數，因為要讓兩個thread使用，所以要設為全域

void *thread_for() { 

	int i=0;
	while(1)
	{ 
		pthread_mutex_lock(&exec_mutex);
		i++;
		pthread_mutex_unlock(&exec_mutex);
		// printf("thread_function is running. i= %d\n",i);
	}
	pthread_exit("Thank you for the CPU time");
}



int main(int argc ,char *argv[]) {
	//printf("argv is %s\n",argv[1]);

	float load = atoi(argv[1])*0.01;//要的負載
	int res;
	int load_sample_time=atoi(argv[2]);//CPU負載取樣時間
	int busy_time;
	int rest_time; 
	if(load<=0.5) //不知道為何算起來會有誤差，因此用這種方法將誤差取消
	{
		busy_time=load_sample_time*load;
		rest_time=load_sample_time-busy_time;
	}
	else
	{
		rest_time=load_sample_time*(1-load);
		busy_time=load_sample_time-rest_time;//(load)*load_sample_time;
	}
	pthread_mutex_init(&exec_mutex,NULL);	

	printf("load=%f,busy_time=%d,rest_time=%d\n",load,busy_time,rest_time);
	pthread_mutex_lock(&exec_mutex);
	pthread_t for_thread;				

	res = pthread_create(&for_thread, NULL, thread_for, NULL);
	if (res != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		pthread_mutex_unlock(&exec_mutex);
		msleep(busy_time); 
		pthread_mutex_lock(&exec_mutex);
		msleep(rest_time);
	}

	exit(EXIT_SUCCESS);
}
