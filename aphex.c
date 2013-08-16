
#include "types.h"

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s <file>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	if (strcmp(argv[1], "-r") == 0) {
		aphexMode = APHEX_READONLY_MODE;
		buf_open((unsigned char*)argv[2]);
	} else {
		aphexMode = APHEX_COMMAND_MODE;
		buf_open((unsigned char*)argv[1]);
	}

	quit = false;
	comNum = 1;
	resetComBuf();
	system("clear");
	if (!aphexContentInit()) {
		printf("terminal too small\n");
		exit(EXIT_SUCCESS);
	}
	cursorX = APHEX_WIN_HEX_X;
	cursorY = APHEX_WIN_HEX_Y;
	while (!quit) {
		aphexWinDraw(&winBase);
		aphexInputProcess();
	}

	buf_close();
	aphexContentFree();
	system("clear");
	exit(EXIT_SUCCESS);
}
