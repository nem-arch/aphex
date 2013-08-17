#include "types.h"

void aphexWinInit(aphexWin *win, int x, int y, int w, int h, bool lf)
{
	win->width = w;
	win->height = h;
	win->posx = x;
	win->posy = y;
	win->c = (char*)calloc(w*h,1);
	aphexWinClear(win,lf);
}
void aphexWinPut(aphexWin *win, char* c, int x, int y) { for (int i=x;i<x+sizeof(c);i++) {
		win->c[i+y*win->width] = c[i-x]; }
}

void aphexWinToWin(aphexWin *base, aphexWin *win)
{
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			base->c[j + win->posx + (base->width) * (i + win->posy)] = win->c[j + (win->width)*i];
		}
	}
}

void aphexWinClear(aphexWin *win, bool lf)
{
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			win->c[j + win->width*i] = ' ';//i/2+'!';
		}
		if (lf) win->c[i*win->width+win->width-1] = '\n';
	}
}

void aphexWinSetTermSize(aphexWin *win)
{
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &aphexTerm)<0) {
		perror("ioctl");
		exit(EXIT_FAILURE);
	}
	if ((aphexTerm.ws_row != win->height) && (aphexTerm.ws_col != win->width)) {
		aphexContentFree();
		aphexContentInit();
		if (cursorY > aphexWinMainBottom()) {
			buf.shiftOffset += cursorY - aphexWinMainBottom() + 1;
			cursorY = aphexWinMainBottom()-1;
		}
		aphexCursorSet(cursorX, cursorY);
	}
}

void aphexWinDraw(aphexWin *win)
{
	aphexWinSetTermSize(&winBase);
	aphexContentBase(&winBase);
	aphexContentAscii(&winAscii);
	aphexContentAddr(&winAddr);
	aphexContentHex(&winHex);
	aphexContentBin(&winBin);
	aphexContentPrompt(&winPrompt);
	system("tput cup 0 0");
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			printf("%c",win->c[j + (win->width)*i]);
		}
	}
	aphexCursorSet(cursorX, cursorY);
}

bool aphexContentInit()
{
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &aphexTerm)<0) {
		perror("ioctl");
		exit(EXIT_FAILURE);
	}
	if (aphexTerm.ws_col < APHEX_WIN_ASCII_X + APHEX_WIN_ASCII_WIDTH) {
		return false;
	}

	aphexWinInit(&winBase, 0, 0, aphexTerm.ws_col, aphexTerm.ws_row-1, true);
	aphexWinInit(&winPrompt, APHEX_WIN_PROMPT_X, aphexWinMainBottom() + APHEX_WIN_BIN_HEIGHT, aphexTerm.ws_col - 2, APHEX_WIN_PROMPT_HEIGHT, false);
	aphexWinInit(&winAddr, APHEX_WIN_ADDR_X, APHEX_WIN_ADDR_Y, APHEX_WIN_ADDR_WIDTH, aphexWinMainBottom() - 1, false);
	aphexWinInit(&winHex, APHEX_WIN_HEX_X, APHEX_WIN_HEX_Y, APHEX_WIN_HEX_WIDTH, aphexWinMainBottom() - 1, false);
	aphexWinInit(&winAscii, APHEX_WIN_ASCII_X, APHEX_WIN_ASCII_Y, APHEX_WIN_ASCII_WIDTH, aphexWinMainBottom() - 1, false);
	aphexWinInit(&winBin, APHEX_WIN_BIN_X, aphexWinMainBottom(), aphexTerm.ws_col - 2, APHEX_WIN_BIN_HEIGHT, false);
	return true;
}

void aphexContentFree()
{
	free(winBase.c);
	free(winPrompt.c);
	free(winAddr.c);
	free(winHex.c);
	free(winAscii.c);
	free(winBin.c);
}

void aphexContentBase(aphexWin *win)
{
}

void aphexContentAddr(aphexWin *win)
{
	char line[APHEX_WIN_ADDR_WIDTH+1] = { ' ' };
	for (int i=0; i<win->height; i++) {
		snprintf(line, win->width+1, "0x%.16lX", i*16+buf.shiftOffset*16);
		strncpy(&(win->c[i*win->width]), line, win->width);
	}
	aphexWinToWin(&winBase, &winAddr);
}

void aphexContentHex(aphexWin *win)
{
	char line[APHEX_WIN_HEX_WIDTH] = { ' ' };
	unsigned long offset = 0;
	for (unsigned long i=0; i<win->height; i++) {
		for (unsigned long j=0; j<8; j++) {
			offset = i*16 + j + 16*buf.shiftOffset;
			if (offset > buf.memsize-1) {
				snprintf(&(line[j*3]), 4, "   ");
			} else {
				snprintf(&(line[j*3]), 4, "%X%X ", c2nH(buf.mem[offset]), c2nL(buf.mem[offset]));
			}
		}
		strncpy(&(line[8*3]), " ", 1);
		for (unsigned long j=0; j<8; j++) {
			offset = i*16 + j + 8 + 16*buf.shiftOffset;
			if (offset > buf.memsize-1) {
				snprintf(&(line[j*3 + 8*3+1]), 4, "   ");
			} else {
				snprintf(&(line[j*3 + 8*3+1]), 4, "%X%X ", c2nH(buf.mem[offset]), c2nL(buf.mem[offset]));
			}
		}
		strncpy(&(win->c[i*win->width]), line, win->width);
	}
	aphexWinToWin(&winBase, &winHex);
}

void aphexContentAscii(aphexWin *win)
{
	char line[APHEX_WIN_ASCII_WIDTH] = { ' ' };
	unsigned long offset = 0;
	for (unsigned long i=0; i<win->height; i++) {
		for (unsigned long j=0; j<8; j++) {
			offset = i*16 + j + 16*buf.shiftOffset;
			if (offset > buf.memsize-1) {
				snprintf(&(line[j]), 2, " ");
			} else {
				snprintf(&(line[j]), 2, "%c", c2a(buf.mem[offset]));
			}
		}
		strncpy(&(line[8]), " ", 1);
		for (unsigned long j=0; j<8; j++) {
			offset = i*16 + j + 8 + 16*buf.shiftOffset;
			if ((i*16+j + 8 + buf.shiftOffset*16) > buf.memsize-1) {
				snprintf(&(line[j+8+1]), 2, " ");
			} else {
				snprintf(&(line[j+8+1]), 2, "%c", c2a(buf.mem[offset]));
			}
		}
		strncpy(&(win->c[i*win->width]), line, win->width);
	}
	aphexWinToWin(&winBase, &winAscii);
}

void aphexContentBin(aphexWin *win)
{
	buf_memBin(&(win->c[win->width-1]));
	aphexWinToWin(&winBase, &winBin);
}

void aphexContentPrompt(aphexWin *win)
{
	snprintf(win->c, win->width, "offset: 0x%.16lX nibble: %s goto: 0x%.16lX", buf.offset, (buf.nibble?"HIGH":"LOW "),comNum);
	snprintf(&win->c[win->width], win->width, "aphex [%c] [%.3i %.3i] shift: %.16lX com: %s",aphexMode,cursorX,cursorY,buf.shiftOffset,comBuf);
	
	aphexWinToWin(&winBase, &winPrompt);
}

int aphexWinMainBottom()
{
	return (aphexTerm.ws_row - 1 -  APHEX_WIN_PROMPT_HEIGHT - APHEX_WIN_BIN_HEIGHT);
}

