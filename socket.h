#ifndef SOCKET_H
#define SOCKET_H

typedef enum _errcode {
	BADCOORD,
	NOTEMPTY,
	BADINPUT,
} errcode_t ;

void
send_msg (int sock_fd, char * data, int nbytes) ;

char *
recv_msg (int sock_fd) ; 

void
send_err (int sock_fd, char * err, errcode_t errcode) ;

#endif
