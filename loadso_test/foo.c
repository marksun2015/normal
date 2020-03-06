#include "foo.h"
#include <stdio.h>

void foo(char *name, int i) {
	char command[256];

	sprintf(command,"name_%s_%d ..... > /dev/kmsg", name);
	system(command);
	//sprintf(command,"/root/network %d", i);
	//system(command);
	//system("echo endnetwork..... > /dev/kmsg\n");
}
