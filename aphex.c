
#include "types.h"

int main(int argc, char **argv)
{
	if (argc < 2 || (strcmp(argv[1], "-h") == 0)) {
		printf("usage: %s <file>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	quit = false;
	system("clear");
	aphexWin base;
	aphexWin addr;
	aphexWinInit(&base,0,0,0,0);
	aphexWinInit(&addr,0,0,10,10);
	buf_open((unsigned char*)argv[1]);
	cursorX = cursorY = 0;
	while (!quit) {
		aphexWinSetTermSize(&base);
		aphexContentPrompt(&base);
		aphexWinDraw(&base);
		process_input();
	}
	buf_close();
	system("clear");
	return 0;
}
