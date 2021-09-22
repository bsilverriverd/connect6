#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "connect6.h"

char ver_arr[20][3] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
						"10", "11", "12", "13", "14", "15", "16", "17", "18", "19" } ;
char buf[10] ;
char * wait ;

char *
generate_string (char hor1, int ver1, char hor2, int ver2)
{
	memset(buf, '\0', sizeof(buf)) ;

	snprintf(buf, 10, "%c%d:%c%d", hor1, ver1, hor2, ver2) ;

	return buf ;
}

int
main (int argc, char * argv[])
{
	srand(time(0x0)) ;

	if (argc != 4) {
		printf("Usage: ./a.out \"server ip address\" \"port number\" \"color\"") ;
		exit(EXIT_SUCCESS) ;
	}
	char * ip = argv[1] ;
	int port = atoi(argv[2]) ;
	char * color = argv[3] ;

	char * redstones = _connect(ip, port, color) ;

	if (strcmp(color, "black") == 0) {
		char * first = draw_and_wait("K10") ;
	} else if (strcmp(color, "white") == 0) {
		char * first = draw_and_wait("") ;
	} else {
		exit(EXIT_FAILURE) ;
	}

	while (1) {
		char hor1 = (rand() % 19) + 'A' ;
		int ver1 = (rand() % 19) + 1 ;
		char hor2 = (rand() % 19) + 'A' ;
		int ver2 = (rand() % 19) + 1 ;
		
		char * msg = generate_string(hor1, ver1, hor2, ver2) ;
		fprintf(stderr, "draw: %s\n", msg) ;

		wait = draw_and_wait(msg) ;

		fprintf(stderr, "wait: %s\n", wait) ;
	}
}
