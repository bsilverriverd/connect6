#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "connect6.h"
#include "socket.h"

/***** private data structures *****/
int sock_fd ;
char * bufptr ;

typedef enum _status {
	EMPTY,
	BLACK,
	WHITE,
	RED
} status_t ;

status_t board[19][19] ; 

status_t player_color ;
status_t opponent_color ;
/**********************************/

/******* private functions ********/
void
set_board(char * stones, status_t color)
{
	char * _stones = strdup(stones) ;
	if (_stones == 0x0) {
		perror("set_board : strdup") ;
		exit(EXIT_FAILURE) ;
	}

	char * tok = strtok(_stones, ":") ;
	while (tok != 0x0) {
		int hor = 0 ;
		int ver = 0 ;
		switch (tok[0]) {
			case 'a' ... 'h' : hor = 19 - (tok[0] - 'a') - 1 ; break ;
			case 'A' ... 'H' : hor = 19 - (tok[0] - 'A') - 1 ; break ;
			case 'j' ... 't' : hor = 19 - (tok[0] - 'a')  ; break ;
			case 'J' ... 'T' : hor = 19 - (tok[0] - 'A')  ; break ;
			default : exit(EXIT_FAILURE) ;//error
		}
		if (strlen(tok) == 3) {
			if (tok[1] == '0' && '1' <= tok[2] && tok[2] <= '9') {
				ver = tok[2] - '0' - 1 ;
			} else if (tok[1] == '1' && '0' <= tok[2] && tok[2] <= '9') {
				ver = 10 + tok[2] - '0' - 1 ;
			} else {
				exit(EXIT_FAILURE) ;
			}
		} else if (strlen(tok) == 2) {
			if ('1' <= tok[2] && tok[2] <= '9') {
				ver = tok[2] - '0' - 1 ;
			} else { 
				//error
				exit(EXIT_FAILURE) ;
			}
		} else {
			//error
			exit(EXIT_FAILURE) ;
		}

		if (board[ver][hor] == EMPTY) {
			board[ver][hor] = color ;
		} else {
			perror("set_board : attempted to set board on a non-empty point") ;
			exit(EXIT_FAILURE) ;
		}
		tok = strtok(0x0, ":") ;
	} // while (tok != 0x0)
	free(_stones) ;	
}
/**********************************/

/******** header functions ********/
char *
_connect(char * ip, int port, char * color)
{
	if (strcmp(color, "black") == 0) {
		player_color = BLACK ;
		opponent_color = WHITE ;
	} else if (strcmp(color, "white") == 0) {
		player_color = WHITE ;
		opponent_color = BLACK ;
	} else {
		exit(EXIT_FAILURE) ;
	}
	
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 19; j++)
			board[i][j] = EMPTY ;

	struct sockaddr_in serv_addr ;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket") ;
		exit(EXIT_FAILURE) ;
	}

	memset(&serv_addr, 0, sizeof(serv_addr)) ;
	serv_addr.sin_family = AF_INET ;
	serv_addr.sin_port = htons(port) ;
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton") ;
		exit(EXIT_FAILURE) ;
	}

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect") ;
		exit(EXIT_FAILURE) ;
	}

#ifdef DEBUG
	fprintf(stderr, "[connect6.c: _connect] Connected!\n") ;
#endif
	// read readstones
	bufptr = recv_msg(sock_fd) ;
#ifdef DEBUG
	fprintf(stderr, "[connect6.: _connect] bufptr:%s\n", bufptr) ;
#endif
	
	set_board(bufptr, RED) ;
	
	return bufptr ;
}

char *
draw_and_wait(char * home)
{
	set_board(home, player_color) ;

	if (strcmp(home, "") != 0)
		send_msg(sock_fd, home, strlen(home)) ;

	bufptr = recv_msg(sock_fd) ;
	
#ifdef DEBUG
	fprintf(stderr, "[connect6.c: draw_and_wait] bufptr: %s\n", bufptr) ;
#endif
	set_board(bufptr, opponent_color) ; 

	return bufptr ;
}

char
get_board(char * ask) {
	int hor = 0 ;
	int ver = 0 ;
	switch (ask[0]) {
		case 'a' ... 'h' : hor = 19 - (ask[0] - 'a') - 1 ; break ;
		case 'A' ... 'H' : hor = 19 - (ask[0] - 'A') - 1 ; break ;
		case 'j' ... 't' : hor = 19 - (ask[0] - 'j') ; break ;
		case 'J' ... 'T' : hor = 19 - (ask[0] - 'J') ; break ;
		default : return 'N' ; 
	}
	if (strlen(ask) == 3) {
		if (ask[1] == '0' && '1' <= ask[2] && ask[2] <= '9') {
			ver = ask[2] - '0' - 1 ;
		} else if (ask[1] == '1' && '0' <= ask[2] && ask[2] <= '9') {
			ver = 10 + ask[2] - '0' - 1 ;
		} else {
			return 'N' ;
		}
	} else if (strlen(ask) == 2) {
		if ('1' <= ask[2] && ask[2] <= '9') {
			ver = ask[2] - '0' - 1 ;
		} else { 
			return 'N' ;
		}
	} else {
		return 'N' ;
	}
	
	switch (board[ver][hor]) {
		case EMPTY : return 'E' ;
		case BLACK : return 'B' ;
		case WHITE : return 'W' ;
		case RED : return 'R' ;
		default : return 'N' ;
	}
}
/**********************************/
