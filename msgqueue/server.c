#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/stat.h>  
#include <sys/msg.h>  
#define MSG_FILE "server.c"  
#define BUFFER 255  
#define PERM S_IRUSR|S_IWUSR  
struct msgtype {  
	long mtype;  
	char buffer[BUFFER+1];  
};  
int main()  
{  
	struct msgtype msg;  
	key_t key;  
	int msgid;  
	if((key=ftok(MSG_FILE,'a'))==-1)  
	{  
		printf("Creat Key Error\n");  
		return 1;  
	}  
	if((msgid=msgget(key,PERM|IPC_CREAT|IPC_EXCL))==-1)  
	{  
		printf("Creat Message Error \n");  
		return 1;  
	}  
	while(1)  
	{  
		msgrcv(msgid,&msg,sizeof(struct msgtype),1,0);  
		printf("Server Receiv\n");  
		msg.mtype=2;  
		msg.buffer[0]='M';  
		msg.buffer[1]='a';  
		msg.buffer[2]='r';  
		msg.buffer[3]='k';  
		msgsnd(msgid,&msg,sizeof(struct msgtype),0);  
	}  
	return 0;  
}
