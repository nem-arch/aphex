
#include "types.h"

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s <file>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	buf_open((unsigned char*)argv[1]);
	buf_close();
	return 0;
}
