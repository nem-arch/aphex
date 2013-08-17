
#include "types.h"

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s [-r] <file>\n", argv[0]);
		printf("-r: open file readonly\n");
		printf("\n");
		printf("maximum filesize on system: %lu MB\n",ULONG_MAX/1024/1024);
		printf("\n");
		printf("up, down, left, right: j, k, h, l\n");
		printf("line start, line end: 0, $\n");
		printf("file top, file bottom: gg, G\n");
		printf("cursor +5 lines: 5j\n");
		printf("cursor -10 lines: 10k\n");
		printf("cursor +20 bytes: 20l\n");
		printf("cursor -51 bytes: 51h\n");
		printf("goto offset 0x10: 16g\n");
		printf("quit: Q\n");
		printf("redraw after resize: q (or any movement)\n");
		printf("input mode: i\n");
		printf("command mode: ESC\n");
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
