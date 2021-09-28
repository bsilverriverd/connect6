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

/**************
	returns 1 on success
	returns 0 on failure
**************/
void
print_board() {
	char visual[] = "*@OX" ;
	for (int ver = 0; ver < 19; ver++) {
		printf("%2d ", 18-ver) ;
		for (int hor = 0; hor < 19; hor++)	{
			printf("%c", visual[board[18-ver][hor]]) ;
		}
		printf("\n") ;
	}
	printf("   ABCDEFGHJKLMNOPQRST\n") ;
}
int
set_redstones(char * redstones)
{
	char * _redstones = strdup(redstones) ;
	char * stone = strtok(_redstones, ":") ;
	while (stone != 0x0) {
		int hor = 0 ;
		int ver = 0 ;
		switch (stone[0]) {
			case 'a' ... 'h' : hor = stone[0] - 'a' ; break ;
			case 'A' ... 'H' : hor = stone[0] - 'A' ; break ;
			case 'j' ... 't' : hor = stone[0] - 'a' - 1 ; break ;
			case 'J' ... 'T' : hor = stone[0] - 'A' - 1 ; break ;
			default : return -1 ;
		}
		if (strlen(stone) == 3) {
			if (stone[1] == '0' && '1' <= stone[2] && stone[2] <= '9') {
				ver = stone[2] - '0' - 1 ;
			} else if (stone[1] == '1' && '0' <= stone[2] && stone[2] <= '9') {
				ver = 10 + stone[2] - '0' - 1 ;
			} else {
				return -1 ;
			}
		} else if (strlen(stone) == 2) {
			if ('1' <= stone[1] && stone[1] <= '9') {
				ver = stone[1] - '0' - 1 ;
			} else { 
				return -1 ;
			}
		} else {
			return -1 ;
		}

		if (board[ver][hor] == EMPTY) {
			board[ver][hor] = RED ;
		} else {
			return -1 ;
		}
		stone = strtok(0x0, ":") ;
	} // while()
	free(_redstones) ;
#ifdef DEBUG
	print_board() ;
#endif
	return 1 ;
	
}

/**************
	returns 1 on success
	returns 0 on white's first move
	return -1 on failure
**************/
int
set_board(char * stones, status_t color)
{
	if ((color == BLACK) && (strcmp(stones, "K10") == 0 || strcmp(stones, "k10") == 0)) {
		board[9][9] = color ;
		return 1 ;
	}
	if ((color == WHITE) && (strcmp(stones, "") == 0)) {
		return 0 ;
	}

	char * _stones = strdup(stones) ;
	if (_stones == 0x0) {
		perror("set_board : strdup") ;
		exit(EXIT_FAILURE) ;
	}

	char * stone[2] ;

	stone[0] = strtok(_stones, ":") ;
	if (stone[0] == 0x0) {
		send_err(sock_fd, stones, BADINPUT) ; // BADINPUT
		return -1 ;
	}
	stone[1] = strtok(0x0, ":") ;
	if (stone[1] == 0x0) {
		send_err(sock_fd, stones, BADINPUT) ; // BADINPUT
		return -1 ;
	}
	char * err = strtok(0x0, ":") ;
	if (err != 0x0) {
		send_err(sock_fd, stones, BADINPUT) ; // BADINPUT
		return -1 ;
	}

	for (int i = 0; i < 2; i++) {
		int hor = 0 ;
		int ver = 0 ;
		switch (stone[i][0]) {
			case 'a' ... 'h' : hor = stone[i][0] - 'a' ; break ;
			case 'A' ... 'H' : hor = stone[i][0] - 'A' ; break ;
			case 'j' ... 't' : hor = stone[i][0] - 'a' - 1 ; break ;
			case 'J' ... 'T' : hor = stone[i][0] - 'A' - 1 ; break ;
			default : send_err(sock_fd, stones, BADCOORD) ; return -1 ; // BADCOORD
		}
		if (strlen(stone[i]) == 3) {
			if (stone[i][1] == '0' && '1' <= stone[i][2] && stone[i][2] <= '9') {
				ver = stone[i][2] - '0' - 1 ;
			} else if (stone[i][1] == '1' && '0' <= stone[i][2] && stone[i][2] <= '9') {
				ver = 10 + stone[i][2] - '0' - 1 ;
			} else {
				send_err(sock_fd, stones, BADCOORD) ; // BADCOORD
				return -1 ;
			}
		} else if (strlen(stone[i]) == 2) {
			if ('1' <= stone[i][1] && stone[i][1] <= '9') {
				ver = stone[i][1] - '0' - 1 ;
			} else { 
				send_err(sock_fd, stones, BADCOORD) ; //BADCOORD
				return -1 ;
			}
		} else {
			send_err(sock_fd, stones, BADINPUT) ; // BADINPUT
			return -1 ;
		}

		if (board[ver][hor] == EMPTY) {
			board[ver][hor] = color ;
		} else {
			send_err(sock_fd, stones, NOTEMPTY) ; // NOTEMPTY
			return -1 ;
		}
	} // for()
	free(_stones) ;	
#ifdef DEBUG
	print_board() ;
#endif
	return 1 ;
}
/**********************************/

/******** header functions ********/
char *
lets_connect(char * ip, int port, char * color)
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
	
	if (set_redstones(bufptr) < 0) {
		perror("bad redstones") ;
		exit(EXIT_FAILURE) ;
	}
	
	return bufptr ;
}

char *
draw_and_wait(char * draw)
{
	if (set_board(draw, player_color) > 0)
		send_msg(sock_fd, draw, strlen(draw)) ;

	bufptr = recv_msg(sock_fd) ;
	
#ifdef DEBUG
	fprintf(stderr, "[connect6.c: draw_and_wait] bufptr: %s\n", bufptr) ;
#endif
	if (strcmp(bufptr, "WIN") != 0 && strcmp(bufptr, "LOSE") != 0) {
		set_board(bufptr, opponent_color) ; 
	}

	return bufptr ;
}

char
get_board(char * ask) {
	int hor = 0 ;
	int ver = 0 ;
	switch (ask[0]) {
		case 'a' ... 'h' : hor = ask[0] - 'a' ; break ;
		case 'A' ... 'H' : hor = ask[0] - 'A' ; break ;
		case 'j' ... 't' : hor = ask[0] - 'a' - 1 ; break ;
		case 'J' ... 'T' : hor = ask[0] - 'A' - 1 ; break ;
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
		if ('1' <= ask[1] && ask[1] <= '9') {
			ver = ask[1] - '0' - 1 ;
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
