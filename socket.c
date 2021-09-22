#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <errno.h>

#include "socket.h"

#define BUFFERSIZE 1024

/** private data structures **/
char buffer[BUFFERSIZE] ;
/*****************************/

/***** private functions *****/
void
send_int (int sock_fd, int data)
{
	int s = 0 ;
	int len = sizeof(int) ;
	char * _data = (char *)&data ;

	while (len > 0 && (s = send(sock_fd, _data, len, 0)) > 0) {
		_data += s ;
		len -= s ;
	}
}

int
recv_int (int sock_fd)
{
	int r = 0 ;
	int len = 0 ;
	int data = 0 ;
	char * _data = (char *)&data ;

	while (len < sizeof(int) && (r = recv(sock_fd, _data, sizeof(int) - len, 0)) > 0) {
		_data += r ;
		len += r ;
	}

	return data ;
}

void
send_nbytes (int sock_fd, char * data, int nbytes)
{
	int s = 0 ;
	int len = nbytes ;
	char * _data = data ;

	while (len > 0 && (s = send(sock_fd, _data, len, 0)) > 0) {
		_data += s ;
		len -= s ;
	}
}

void
recv_nbytes (int sock_fd, char * buf, int nbytes)
{
	memset(buf, 0, nbytes) ;

	int len = 0 ;
	int r = 0 ;

	while (len < nbytes && (r = recv(sock_fd, buf, nbytes, 0)) > 0) {
		buf += r ;
		len += r ;
	}
}
/****************************/

/***** header functions *****/
void
send_msg (int sock_fd, char * msg, int msglen)
{
	send_int(sock_fd, msglen) ;

	send_nbytes (sock_fd, msg, msglen) ;
}

char *
recv_msg (int sock_fd)
{
	int msglen = recv_int(sock_fd) ;
	if (BUFFERSIZE <= msglen) {
		//error	
		exit(EXIT_FAILURE) ;
	}
	
	recv_nbytes(sock_fd, buffer, msglen) ;

	return buffer ;	
}
/****************************/
