#include "types.h"

void win_checksize()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wnew);
	if ((wnew.ws_row != wold.ws_row) || (wnew.ws_col != wold.ws_col)) {
		system("clear");
		wold = wnew;
		win_redraw();
	}
}

void win_redraw()
{
	system("tput cup 0 0");
}

