/*
 *   $ gcc -o modbustcp_slave modbustcp_slave.c -I/usr/include/modbus -lmodbus
 */

#include <stdio.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <signal.h>  
#include <modbus.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
  
#define NB_CONNECTION    5  
  
modbus_t *ctx = NULL;  
int server_socket;  
modbus_mapping_t *mb_mapping;  
  
static void close_sigint(int dummy)  
{  
	close(server_socket);  
	modbus_free(ctx);  
	modbus_mapping_free(mb_mapping);  
  
	exit(dummy);  
}  
  
int main(void)  
{  
	int master_socket;  
	int rc;  
	fd_set refset;   //fd_set现在一共可以监听1024个文件描述符  
	fd_set rdset;  
  
	/*文件描述符的最大值，要保证fdmax是需要监视的最大的文件描述符值*/  
	int fdmax;  
  
	ctx = modbus_new_tcp("192.168.1.179", 1502);
  
	mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,  
									MODBUS_MAX_READ_REGISTERS, 0);  
	if (mb_mapping == NULL) {  
		fprintf(stderr, "Failed to allocate the mapping: %s\n",  
				modbus_strerror(errno));  
		modbus_free(ctx);  
		return -1;  
	}  
  
	server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);  //这儿并不会阻塞  
	/** 
	 * 这儿的server_socket值应该是3，因为标准输入流的文件描述符为0，标准输出流的文件描述符为1 
	 * 标准错误流的文件描述符为2，server_socket是本程序中的第一个文件描述符，所以其值为3 
	 */  
  
  
	/** 
	 * 绑定SIGINT信号到处理函数 
	 * SIGINT信号由Interrupt Key产生，通常是CTRL+C或者DELETE。 
	 * 发送给所有ForeGround Group的进程 
	 */  
	signal(SIGINT, close_sigint);    
  
	/*清空监视集合*/  
	FD_ZERO(&refset);  
  
	/*将server_socket文件描述符加入到监视集合中*/  
	FD_SET(server_socket, &refset);  
  
	/*保证fdmax是需要监视的最大的文件描述符值*/  
	fdmax = server_socket;  
  
	for (;;) {  
		/** 
		 * select函数成功返回时会将未准备好的描述位清零，所以每次select开始之前都需要将refset的值重新赋值给rdset 
		 */  
		rdset = refset;     
  
	/** 
		 * select(int maxfd, fd_set *rdset, fd_set *wrset, fd_set *exset, struct timeval *timeout);  
		 * maxfd是需要监视的最大的文件描述符值+1（这个参数是为了优化性能，否则系统每一次要从0检测到1023） 
		 * select函数用于在非阻塞中，当一个套接字或一组套接字有信号时通知你，系统提供select函数来实现多路复用输入/输出模型 
		 * rdset, wrset, exset分别对应于需要检测的可读文件描述符的集合，可写文件描述符的集合及异常文件描述符的集合，不需要监视的项可以设为NULL 
		 * struct timeval 结构用于描述一段时间长度，如果在这个时间内，需要监视的描述符没有发生变化则函数返回，返回值0。 
		 * （若timeout参数为NULL,则程序一直阻塞直到监视的描述符发生变化） 
		 * FD_ZERO(fd_set *fdset);将指定的文件描述符清空，在对文件描述符进行设置前，必须对其进行初始化。 
		 * FD_SET(fd_set *fdset);用于在文件描述符集合中增加一个新的文件描述符 
		 * FD_CLR(fd_set *fdset);用于在文件描述符集合中删除一个文件描述符 
		 * FD_ISSET(int fd, fd_set *fdset);用于测试指定的文件描述符是否存在该集合中 
		 * 过去，一个fd_set通常只能包含小于32的fd，因为fd_set其实只用了一个32位矢量来表示fd;  
		 * 现在UNIX系统通常会在头文件<sys/select.h>中定义常量FD_SETSIZE,它是数据类型fd_set的描述自数量，其值通常是1024, 
		 * 这样就能表示小于1024的fd 
		 */  
		if (select(fdmax+1, &rdset, NULL, NULL, NULL) == -1) {     
			perror("Server select() failure.");  
			close_sigint(1);  
		}  
  
		/**其实master_socket=0处完全可以换为3，因为0/1/2号设备时系统的标准输入/输出/错误流，并不需要监视**/  
		for (master_socket = 0; master_socket <= fdmax; master_socket++) {      
			if (FD_ISSET(master_socket, &rdset)) {  //检查master_socket是否存在该集合中,即对应的描述符是否发生变化  
				if (master_socket == server_socket) { //socket客户端请求一个新的socket连接  
					socklen_t addrlen;  
					struct sockaddr_in clientaddr;  
					int newfd;  
  
					/*处理新的连接请求*/  
					addrlen = sizeof(clientaddr);  
					memset(&clientaddr, 0, sizeof(clientaddr));  
					newfd = accept(server_socket, (struct sockaddr *)&clientaddr, &addrlen);  
  
					if (newfd == -1) {  
						perror("Server accept() error");  
					} else {  
						FD_SET(newfd, &refset);   //将新连接的文件描述符加入监视集合中  
  
						if (newfd > fdmax) {  
							/*保证fdmax是需要监视的最大的文件描述符值*/  
							fdmax = newfd;  
						}  
						printf("New connection from %s:%d on socket %d\n",  
							   inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);  
					}  
				} else {//一个已连接的socket客户端（modbus主机）发送来了新的请求报文  
					uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];  
			//处理请求报文  
					modbus_set_socket(ctx, master_socket);  
					rc = modbus_receive(ctx, query);  
					if (rc != -1) {  
						modbus_reply(ctx, query, rc, mb_mapping);  
					} else {  
						/*读取出错，则说明连接断开或发生了不可恢复的错误，应该释放连接*/  
						printf("Connection closed on socket %d\n", master_socket);  
						close(master_socket);  
  
						/*将释放掉的连接的文件描述符从监视集合中清除*/  
						FD_CLR(master_socket, &refset);  
  
						if (master_socket == fdmax) {  
							/*保证fdmax是需要监视的最大的文件描述符值*/  
							fdmax--;  
						}  
					}  
				}  
			}  
		}  
	}  
  
	return 0;  
}  
