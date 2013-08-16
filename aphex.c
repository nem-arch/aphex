
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
	aphexMode = APHEX_COMMAND_MODE;
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
