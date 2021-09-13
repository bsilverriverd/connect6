#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connect6.h"

typedef struct
_board {
	int board[19][19] ; 
} board_t ;

int sock_fd ;

/********************** 
	return 1 on success
	return 0 in failure
***********************/
int
connect(char * ip, int port, char * color, char * starting_board)
{
	struct sockaddr_in serv_addr ;

	sock_fd = socket(AF_INET, sSOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket") ;
		exit(EXIT_FAILURE) ;
	}

	memset(&serv_addr, 0, sizeof(serv_addr)) ;
	serv_addr.sin_family = AF_INET ;
	serv_addr.sin_port = htons(port) ;
	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton") ;
		exit(EXIT_FAILURE) ;
	}

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect") ;
		exit(EXIT_FAILURE) ;
	}
}

/*********************************
	return 1 on success
	return error code on failure
**********************************/
int
draw_and_wait(char * home, char * away)
{
	
}

int
get_board(char * ask, ) ;

#endif
