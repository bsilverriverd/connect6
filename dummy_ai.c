#include <stdio.h>

#include "connect6.h"

int
main (int argc, char * argv[])
{
	connect() ;

	if ("BLACK")
		draw_and_wait("K10") ;
	else
		draw_and_wait("") ;

	while (connected) {
		draw_and_wait() ;
	}
}
