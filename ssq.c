
#include <stdlib.h>

#include "core.h"

int main(int argc, char *argv[])
{
	if (core_init() != 0)
		exit(EXIT_FAILURE);
		
	core_run();
		
	core_shutdown();
	
	exit(EXIT_SUCCESS);
}
