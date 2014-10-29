#include <core/io.h>

int raise(int sig)
{
	(void)sig;

	kprint("Error: division by zero attempted\n");
	kprint("STOPPED\n");

	while(1);

	return 0;
}
