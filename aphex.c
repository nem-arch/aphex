#include <sys/mman.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s <file>\n", argv[0]);
	}
	return 0;
}
