#include "types.h"

void aphexWinInit(aphexWin *win, int x, int y, int w, int h)
{
	win->c = (char*)malloc(w*h+w+h);
	win->width = w;
	win->height = h;
	win->posx = x;
	win->posy = y;
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
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			base->c[j + win->posx + (base->width) * (i + win->posy + 1)] = win->c[j + (win->width)*i];
			//base->c[j + (base->width) * (i)] = win->c[j + (win->width)*i]; 
		}
	}
}

void aphexWinClear(aphexWin *win)
{
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			win->c[j + win->width*i] = ' ';
		}
		win->c[i*win->width+win->width-1] = '\n';
	}
}

void aphexWinSetTermSize(aphexWin *win)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &aphexTerm);
	if ((aphexTerm.ws_row != win->height) && (aphexTerm.ws_col != win->width)) {
		aphexContentFree();
		aphexContentInit();
		if (cursorY > aphexWinMainBottom()) {
			buf.shiftOffset += cursorY - aphexWinMainBottom() + 1;
			cursorY = aphexWinMainBottom() - 1;
		}
		aphexCursorSet(cursorX, cursorY);
	}
}

void aphexWinDraw(aphexWin *win)
{
	aphexWinSetTermSize(&winBase);
	aphexContentBase(&winBase);
	aphexContentHex(&winHex);
	aphexContentAddr(&winAddr);
	aphexContentAscii(&winAscii);
	aphexContentBin(&winBin);
	aphexContentPrompt(&winPrompt);
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			printf("%c",win->c[j + (win->width)*i]);
		}
	}
	aphexCursorSet(cursorX, cursorY);
}

bool aphexContentInit()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &aphexTerm);
	if (aphexTerm.ws_col < APHEX_WIN_ASCII_X + APHEX_WIN_ASCII_WIDTH) {
		return false;
	}

	aphexWinInit(&winBase, 0, 0, aphexTerm.ws_col, aphexTerm.ws_row);
	aphexWinInit(&winPrompt, APHEX_WIN_PROMPT_X, aphexWinMainBottom() + APHEX_WIN_BIN_HEIGHT, aphexTerm.ws_col, APHEX_WIN_PROMPT_HEIGHT);
	aphexWinInit(&winAddr, APHEX_WIN_ADDR_X, APHEX_WIN_ADDR_Y, APHEX_WIN_ADDR_WIDTH, aphexWinMainBottom() - 1);
	aphexWinInit(&winHex, APHEX_WIN_HEX_X, APHEX_WIN_HEX_Y, APHEX_WIN_HEX_WIDTH, aphexWinMainBottom() - 1);
	aphexWinInit(&winAscii, APHEX_WIN_ASCII_X, APHEX_WIN_ASCII_Y, APHEX_WIN_ASCII_WIDTH, aphexWinMainBottom() - 1);
	aphexWinInit(&winBin, APHEX_WIN_BIN_X, aphexWinMainBottom(), aphexTerm.ws_col, APHEX_WIN_BIN_HEIGHT);
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
	char line[APHEX_WIN_ADDR_WIDTH + 1];
	for (int i=0; i<win->height; i++) {
		snprintf(line, APHEX_WIN_ADDR_WIDTH+1, "0x%.16lX\n", i*16+buf.shiftOffset*16);
		strncpy(&(win->c[i*win->width]), line, APHEX_WIN_ADDR_WIDTH);
	}
	aphexWinToWin(&winBase, &winAddr);
}

void aphexContentHex(aphexWin *win)
{
	char line[APHEX_WIN_HEX_WIDTH + 1];
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<8; j++) {
			if ((i*16+j + buf.shiftOffset*16) > buf.memsize-1) {
				snprintf(&(line[j*3+0]), 2, " \n");
				snprintf(&(line[j*3+1]), 2, " \n");
				if (j<7) snprintf(&(line[j*3+2]), 2, " \n");
				else snprintf(&(line[j*3+2]), 3, "  \n");
			} else {
				snprintf(&(line[j*3+0]), 2, "%.1X\n", c2nH(buf.mem[i*16+j + buf.shiftOffset*16]));
				snprintf(&(line[j*3+1]), 2, "%.1X\n", c2nL(buf.mem[i*16+j + buf.shiftOffset*16]));
				if (j<7) snprintf(&(line[j*3+2]), 2, " \n");
				else snprintf(&(line[j*3+2]), 3, "  \n");
			}
		}
		for (int j=0; j<8; j++) {
			if ((i*16+j+8 + buf.shiftOffset*16) > buf.memsize-1) {
				snprintf(&(line[j*3+0+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, " \n");
				snprintf(&(line[j*3+1+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, " \n");
				if (j<7) snprintf(&(line[j*3+2+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, " \n");
				else snprintf(&(line[j*3+2+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, "\n");
			} else {
				snprintf(&(line[j*3+0+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, "%.1X\n", c2nH(buf.mem[i*16+j+8 + buf.shiftOffset*16]));
				snprintf(&(line[j*3+1+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, "%.1X\n", c2nL(buf.mem[i*16+j+8 + buf.shiftOffset*16]));
				if (j<7) snprintf(&(line[j*3+2+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, " \n");
				else snprintf(&(line[j*3+2+APHEX_WIN_HEX_BLOCK_WIDTH+2]), 2, "\n");
			}
		}
		strncpy(&(win->c[i*win->width]), line, APHEX_WIN_HEX_WIDTH);
	}
	aphexWinToWin(&winBase, &winHex);
}

void aphexContentAscii(aphexWin *win)
{
	char line[APHEX_WIN_ASCII_WIDTH + 1];
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<8; j++) {
			if ((i*16+j + buf.shiftOffset*16) > buf.memsize-1) {
				snprintf(&(line[j]), 2, " \n");
				if (j==7) snprintf(&(line[j+1]), 3, " \n");
			} else {
				snprintf(&(line[j]), 2, "%c\n", c2a(buf.mem[i*16+j + buf.shiftOffset*16]));
				if (j==7) snprintf(&(line[j+1]), 3, " \n");
			}
		}
		for (int j=0; j<8; j++) {
			if ((i*16+j+8 + buf.shiftOffset*16) > buf.memsize-1) {
				snprintf(&(line[j+8+1]), 2, " \n");
				if (j==7) snprintf(&(line[j+8+2]), 2, "\n");
			} else {
				snprintf(&(line[j+8+1]), 2, "%c\n", c2a(buf.mem[i*16+j+8 + buf.shiftOffset*16]));
				if (j==7) snprintf(&(line[j+8+2]), 2, "\n");
			}
		}
		strncpy(&(win->c[i*win->width]), line, APHEX_WIN_ASCII_WIDTH);
	}
	aphexWinToWin(&winBase, &winAscii);
}

void aphexContentBin(aphexWin *win)
{
	unsigned char b[win->width];
	memset(b, ' ', win->width-1);
	buf_memBin(b);
	for (int i=0; i<win->width-1; i++) {
		// only draw middle line
		win->c[i+win->width + 1] = b[i];
	}
	win->c[win->width*1-1] = '\n';
	win->c[win->width*2-1] = '\n';
	win->c[win->width*3-1] = '\n';
	aphexWinToWin(&winBase, &winBin);
}

void aphexContentPrompt(aphexWin *win)
{
	char p[win->width*win->height];
	memset(p, '\n', win->width*win->height-1);
	snprintf(p, win->width-1, " offset: 0x%.16lX nibble: %s goto: 0x%.16lX\n", buf.offset, (buf.nibble?"HIGH":"LOW "),comNum);
	snprintf(&p[win->width], win->width-1, " aphex [%c] [%.3i %.3i] shift: %.16lX com: %s\n",aphexMode,cursorX,cursorY,buf.shiftOffset,comBuf);
	//snprintf(win->c, (aphexTerm.ws_col-2)*2, p);
	//snprintf(&win->c[(aphexTerm.ws_col-2)*2-1], 2, "\n");
	
	for (int i=0; i<win->height; i++) {
		int j=0;
		while (1) {
			if (p[j+win->width*i]=='\n') break;
			win->c[j+win->width*i] = p[j+win->width*i];
			j++;
		}
	}
	/*
	for (int i=0; i<win->width*2; i++) {
		win->c[i] = p[i];
	}
	win->c[(win->height*win->width)-1] = '\n';
	*/
	aphexWinToWin(&winBase, &winPrompt);
}

int aphexWinMainBottom()
{
	return (aphexTerm.ws_row - 2 - APHEX_WIN_PROMPT_HEIGHT - APHEX_WIN_BIN_HEIGHT);
}

