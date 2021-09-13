#ifndef CONNECT6_H
#define CONNECT6_H

/********************** 
	Return 1 on success
	Return 0 on failure

	Request TCP connection to connect6 server at ip/port as color.
	Returns redstones via red_stones
***********************/
int
connect(char * ip, int port, char * color, char * red_stones) ;

/**********************
	Return 1 on success
	Return error code on failure

	String format example: A01:B01(fixed 7 bytes)

	If home is "K10" or "", skip drawing and just wait
	Black must draw_and_wait("K10", away) on first move
	White must draw_and_waiat("", away) on first move
***********************/
int
draw_and_wait(char * home, char * away) ;

/***********************
	get status of board at given coordinate
	Black = B
	White = W
	Empty = E
	Red   = R
	Error = N // ex) out of bounds
************************/
char
get_board(char * ask) ;

#endif
