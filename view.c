#include "types.h"

void aphexWinInit(aphexWin *win, int x, int y, int w, int h)
{
	win->c = (char*)malloc((w+1)*h);
	if ((w == 0) && (h == 0)) {
		aphexWinSetTermSize(win);
	} else {
		win->width = w;
		win->height = h;
	}
	aphexWinClear(win);
}

void aphexWinPut(aphexWin *win, char* c, int x, int y)
{
	for (int i=x;i<x+sizeof(c);i++) {
		win->c[i+y*win->width] = c[i-x];
	}
}

void aphexWinToWin(aphexWin *base, aphexWin *win)
{
	for (int i=0;i<win->height;i++) {
		for (int j=0;j<win->width;j++) {
			base->c[j+win->posx+base->width*(i+win->posy)] = win->c[j+win->width*i];
		}
	}
}

void aphexWinClear(aphexWin *win)
{
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			win->c[j + win->width*i] = ' ';
		}
		win->c[win->width*(i+1)-1] = '\n';
	}
}

void aphexWinSetTermSize(aphexWin *win)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &aphexTerm);
	win->width = aphexTerm.ws_col;
	win->height = aphexTerm.ws_row;
}

void aphexWinDraw(aphexWin *win)
{
	system("tput cup 0 0");
	//printf("%s",win->c);
	//cursor_set(cursorX, cursorY);
	//return;
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			printf("%c",win->c[j + win->width*i]);
		}
	}
	cursor_set(cursorX, cursorY);
}

void aphexContentPrompt(aphexWin *win)
{
	char p[128];
	sprintf(p,"aphex [%i %i]>",cursorX,cursorY);
	strncpy(&(win->c[((win->height-2) * win->width)]),p,sizeof(p)-1);
}

