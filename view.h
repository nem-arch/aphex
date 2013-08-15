#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

typedef struct _aphexWin {
	int posx;
	int posy;
	int width;
	int height;
	char *c;
} aphexWin;

void aphexWinInit(aphexWin *win, int x, int y, int w, int h);
void aphexWinPut(aphexWin *win, char* c, int x, int y);
void aphexWinToWin(aphexWin *base, aphexWin *win);
void aphexWinClear(aphexWin *win);
void aphexWinDraw(aphexWin *win);

/* termsize buffer for ioctl */
struct winsize aphexTerm;

/* get termsize from ioctl and set aphexWin */
void aphexWinSetTermSize(aphexWin *win);

void aphexContentPrompt(aphexWin *win);

#endif // VIEW_H_INCLUDED
