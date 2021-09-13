#ifndef CONNECT6_H
#define CONNECT6_H

typedef struct
_board {
	int board[19][19] ; 
} board_t ;

/********************** 
	return 1 on success
	return 0 in failure
***********************/
int
connect(char * ip, int port, char * color, char * starting_board) ;

/*********************************
	return 1 on success
	return error code on failure
**********************************/
int
draw_and_wait(char * home, char * away) ;

/***********************
	get api maintained board
************************/
int
get_board(char * ask, ) ;

#endif
