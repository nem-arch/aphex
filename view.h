#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

// LAYOUT DEFINITIONS

#define APHEX_WIN_ORIGIN_X (1)
#define APHEX_WIN_ORIGIN_Y (1)

// offset addresses at origin
#define APHEX_WIN_ADDR_X (APHEX_WIN_ORIGIN_X)
#define APHEX_WIN_ADDR_Y (APHEX_WIN_ORIGIN_Y)
#define APHEX_WIN_ADDR_WIDTH (10)

// hex window to the right of offsets
#define APHEX_WIN_HEX_X (APHEX_WIN_ADDR_X + APHEX_WIN_ADDR_WIDTH + 2)
#define APHEX_WIN_HEX_Y (APHEX_WIN_ORIGIN_Y)
#define APHEX_WIN_HEX_BLOCK_WIDTH (23) // (2 per byte)*(8 bytes) + (7 spaces)
#define APHEX_WIN_HEX_WIDTH (APHEX_WIN_HEX_BLOCK_WIDTH*2 + 2)

// ascii window to the right of hex
#define APHEX_WIN_ASCII_X (APHEX_WIN_HEX_X + APHEX_WIN_HEX_WIDTH + 2)
#define APHEX_WIN_ASCII_Y (APHEX_WIN_ORIGIN_Y)
#define APHEX_WIN_ASCII_BLOCK_WIDTH (8)
#define APHEX_WIN_ASCII_WIDTH (APHEX_WIN_ASCII_BLOCK_WIDTH*2 + 1)

// binary window at the bottom spanning full width
#define APHEX_WIN_BIN_X (APHEX_WIN_ORIGIN_X)
#define APHEX_WIN_BIN_HEIGHT (2)

// WINDOW STRUCTURE AND FUNCTIONS

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

/*
 *	APHEX CONTENT
 */

aphexWin winBase;
aphexWin winAddr;
aphexWin winHex;
aphexWin winAscii;
aphexWin winBin;

void aphexContentInit();
void aphexContentFree();
void aphexContentBase(aphexWin *win);
void aphexContentAddr(aphexWin *win);
void aphexContentHex(aphexWin *win);
void aphexContentAscii(aphexWin *win);
void aphexContentBin(aphexWin *win);
void aphexContentPrompt(aphexWin *win);

#endif // VIEW_H_INCLUDED
