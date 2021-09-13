#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connect6.h"

int sock_fd ;

typedef enum _status {
	EMPTY,
	BLACK,
	WHITE,
	RED
} status_t ;

typedef struct
_board {
	 board[19][19] ; 
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

	// TODO
	// read readstones

	return 1 ;
}

int
draw_and_wait(char * home, char * away)
{
	if (strcmp(home, "") != 0 || strcmp(home, "K10") != 0) {
		// TODO
		// send home
	}

	// TODO
	// read board coordinates
}

char
get_board(char * ask) {
	// TODO
	// get coordinates
	int ver = 0 ;
	int hor = 0 ;
	char ver_tok = '\0' ;
	int hor_tok = 0 ;
	sscanf(ask, "%c %d", &ver_tok, hor_tok) ;

	if ('A' <= ver_tok && ver_tok <= 'H') {
		ver = ver_tok - 'A' ;
	} else if ('J' <= ver_tok && ver_tok <= 'T') {
		ver = ver_tok - 'A' - 1 ;
	} else {
		return 'N' ;
	}

	if (1 <= hor_tok && hor_tok <= 19) {
		hor = hor_tok - 1 ;
	} else {
		return 'N' ;
	}

	char status = 'N' ;
	switch (board.board[hor][ver]) {
		case EMPTY: status = 'E' ; break ;
		case BLACK: status = 'B' ; break ;
		case WHITE: status = 'W' ; break ;
		case RED: status = 'R' ; break ;
		default: status = 'N' ; break ;
	}
	return status ;
}

#endif
