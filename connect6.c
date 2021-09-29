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

char *
status_str[5] = {
	"EMPTY",
	"BLACK",
	"WHITE",
	"RED",
	"ERROR",
} ;
/**********************************/

/******* private functions ********/

/**************
	returns 1 on success
	returns 0 on failure
**************/
void
print_board() {
	char visual[] = "*@OX" ;
	for (int ver = 19; ver > 0; ver--) {
		printf("%2d ", ver) ;
		for (int hor = 0; hor < 19; hor++)	{
			printf("%c", visual[board[ver-1][hor]]) ;
		}
		printf("\n") ;
	}
	printf("   ABCDEFGHJKLMNOPQRST\n") ;
}

int
parse (char * stone, int * hor, int * ver)
{
	char a = '\0' ;
	int _hor = -1 ;
	int _ver = -1 ;
	int n = 0 ;
	if (sscanf(stone, "%c%2d%n", &a, &_ver, &n) != 2 || stone[n] != '\0')
		return BADINPUT ;
	
	switch (a) {
		case 'a' ... 'h' : _hor = a - 'a' ; break ;
		case 'A' ... 'H' : _hor = a - 'A' ; break ;
		case 'j' ... 't' : _hor = a - 'a' - 1 ; break ;
		case 'J' ... 'T' : _hor = a - 'A' - 1 ; break ;
		default : return BADCOORD ;
	}
	
	if (0 < _ver && _ver <= 19)
		_ver-- ;
	else
		return BADCOORD ;
	
	*hor = _hor ;
	*ver = _ver ;

	return 0 ;	
}

int
set_redstones (char * redstones)
{
	char * _redstones = strdup(redstones) ;
	char * stone = strtok(_redstones, ":") ;
	while (stone != 0x0) {

		int hor = -1 ;
		int ver = -1 ;
		int err = parse(stone, &hor, &ver) ;
		if (err) {
			perror("set_redstones() : bad redstones") ;
			exit(EXIT_FAILURE) ;
		}
		
		if (board[ver][hor] == EMPTY)
			board[ver][hor] = RED ;
		else
			return -1 ;

		stone = strtok(0x0, ":") ;
	} // while()
	free(_redstones) ;
#ifdef DEBUG
	print_board() ;
#endif
	return 0 ;
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

#ifdef DEBUG
	fprintf(stderr, "[set_board] stone[0] %s\n", stone[0]) ;
	fprintf(stderr, "[set_board] stone[1] %s\n", stone[1]) ;
#endif
	for (int i = 0; i < 2; i++) {
		int hor = -1 ;
		int ver = -1 ;
		int err = parse(stone[i], &hor, &ver) ;
		if (err) {
			send_err(sock_fd, stones, err) ;
			return -1 ;
		}

		if (board[ver][hor] == EMPTY) {
			board[ver][hor] = color ;
		} else {
			send_err(sock_fd, stones, NOTEMPTY) ;
			return -1 ;
		}

#ifdef DEBUG
	fprintf(stderr, "set board[%d][%d] %s\n", ver, hor, get_board(stone[i])) ;
#endif
	}
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

char *
get_board(char * ask) {
	int hor = -1 ;
	int ver = -1 ;
	int err = parse(ask, &hor, &ver) ;
	if (err)
		return status_str[4] ;

#ifdef DEBUG
	fprintf(stderr, "get board[%d][%d] %s\n", ver, hor, status_str[board[ver][hor]]) ;
#endif
	switch (board[ver][hor]) {
		case EMPTY : 
		case BLACK :
		case WHITE :
		case RED : return status_str[board[ver][hor]] ;
		default : return status_str[4] ;
	}
}
/**********************************/
