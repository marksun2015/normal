#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char **argv)
{
	long long size;
	char *a = NULL;

	size = 1024L*1024L*1024L*14L;

	a = malloc(size);

	assert(a);
}

