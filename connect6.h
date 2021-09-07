#ifndef CONNECT6_H
#define CONNECT6_H

#define X(stone) stone/19
#define Y(stone) stone%19

enum
color {
	BLACK,
	WHITE
} ;

typedef struct
_board {
	int coordinates[361] ; 
} board_t ;

/********************** 
	return 1 on success
	return 0 in failure
***********************/
int
connect(char * ip, int port, color c, board_t * starting_board) ;

/*********************************
	return 1 on success
	return error code on failure
**********************************/
int
draw_and_wait(int home1, int home2, int * away1, int * away2) ;

/***********************
	get api maintained board
************************/
board_t
get_board() ;

#endif
