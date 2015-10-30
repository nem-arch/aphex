
#include "types.h"

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s [-r] <file>\n", argv[0]);
		printf("type 'man aphex' for controls and notes\n");
		printf("\n");
		printf("maximum filesize on system: %lu MB\n",LONG_MAX/1024/1024);
		exit(EXIT_SUCCESS);
	}
	if (strcmp(argv[1], "-r") == 0) {
		aphexMode = APHEX_READONLY_MODE;
		buf_open((unsigned char*)argv[2]);
	} else {
		aphexMode = APHEX_COMMAND_MODE;
		buf_open((unsigned char*)argv[1]);
	}

	aphexEditMode = APHEX_EDIT_HEX;
	aphexEditForward = true;
	quit = false;
	comNum = 1;
	resetComBuf();
	// clear screen
	printf("\033[2J\0330;0H");
	fflush(stdout);
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
	printf("\033[0;0H\033[2J\033[0;0H");
	fflush(stdout);
	exit(EXIT_SUCCESS);
}
