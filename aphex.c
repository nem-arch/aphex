
#include "types.h"

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s <file>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	quit = false;
	system("clear");
	buf_open((unsigned char*)argv[1]);
	aphexContentInit();
	cursorX = cursorY = 0;
	while (!quit) {
		aphexContentPrompt(&winBase);
		aphexContentHex(&winHex);
		aphexContentAddr(&winAddr);
		aphexContentAscii(&winAscii);
		aphexWinDraw(&winBase);
		process_input();
	}

	buf_close();
	aphexContentFree();
	system("clear");
	exit(EXIT_SUCCESS);
}
