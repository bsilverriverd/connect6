#ifndef SOCKET_H
#define SOCKET_H

void
send_msg (int sock_fd, char * data, int nbytes) ;

char *
recv_msg (int sock_fd) ; 

#endif
