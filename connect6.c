#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connect6.h"

int sock_fd ;

typedef struct
_board {
	int board[19][19] ; 
} board_t ;

board_t board ;

int
connect(char * ip, int port, char * color, char * red_stones)
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

int
draw_and_wait(char * home, char * away)
{
	if ()
}

char
get_board(char * ask) ;

#endif
