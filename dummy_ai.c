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

	snprintf(buf, 10, "%c%02d:%c%02d", hor1, ver1, hor2, ver2) ;

	return buf ;
}

int
is_empty(char hor, int ver)
{
	char ask[4] ;
	snprintf(ask, 4, "%c%02d", hor, ver) ;

	char c = get_board(ask) ;

	if (c == 'E')
		return 1 ;
	else
		return 0 ;
}

int
main (int argc, char * argv[])
{
	srand(time(0x0)) ;

	if (argc != 4) {
		fprintf(stderr, "Usage: ./a.out \"server ip address\" \"port number\" \"color\"") ;
		exit(EXIT_SUCCESS) ;
	}
	char * ip = argv[1] ;
	int port = atoi(argv[2]) ;
	char * color = argv[3] ;

	char * redstones = lets_connect(ip, port, color) ;
	printf("[main] redstone: %s\n", redstones) ;

	char * first ;
	if (strcmp(color, "black") == 0) {
		first = draw_and_wait("K10") ;
	} else if (strcmp(color, "white") == 0) {
		first = draw_and_wait("") ;
	} else {
		exit(EXIT_FAILURE) ;
	}
	printf("[main] first: %s\n", first) ;

	while (1) {
		char hor1 = '\0' ;
		char hor2 = '\0' ;
		int ver1 = 0 ;
		int ver2 = 0 ;
		do {
			hor1 = (rand() % 20) + 'A' ;
			hor1 += (hor1 == 'I') ? 1 : 0 ;
			ver1 = (rand() % 19) + 1 ;
			do {	
				hor2 = (rand() % 20) + 'A' ;
				hor2 += (hor2 == 'I') ? 1 : 0 ;
				ver2 = (rand() % 19) + 1 ;
			} while (hor1 == hor2 && ver1 == ver2) ;
		} while (!is_empty(hor1, ver1) && !is_empty(hor2, ver2)) ;
		
		char * msg = generate_string(hor1, ver1, hor2, ver2) ;
		printf("\n[main] draw: %s\n", msg) ;

		wait = draw_and_wait(msg) ;

		printf("[main] wait: %s\n", wait) ;
	}
}
