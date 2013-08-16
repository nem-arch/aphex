#include "types.h"

void aphexWinInit(aphexWin *win, int x, int y, int w, int h)
{
	win->c = (char*)malloc(w*h);
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
	win->width = aphexTerm.ws_col;
	win->height = aphexTerm.ws_row;
}

void aphexWinDraw(aphexWin *win)
{
	aphexContentHex(&winHex);
	aphexContentAddr(&winAddr);
	aphexContentAscii(&winAscii);
	aphexContentPrompt(&winBase);
	aphexWinSetTermSize(&winBase);
	for (int i=0; i<win->height; i++) {
		for (int j=0; j<win->width; j++) {
			printf("%c",win->c[j + (win->width)*i]);
		}
	}
	aphexCursorSet(cursorX, cursorY);
	//aphexCursorSet(APHEX_WIN_HEX_X + ((buf.offset)%16)*3 + (buf.nibble^APHEX_NIBBLE_HIGH), buf.offset/16 + APHEX_WIN_HEX_Y);
}

void aphexContentInit()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &aphexTerm);

	aphexWinInit(&winBase, 0, 0, aphexTerm.ws_col, aphexTerm.ws_row);
	aphexWinInit(&winAddr, APHEX_WIN_ADDR_X, APHEX_WIN_ADDR_Y, APHEX_WIN_ADDR_WIDTH, aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT - 1);
	aphexWinInit(&winHex, APHEX_WIN_HEX_X, APHEX_WIN_HEX_Y, APHEX_WIN_HEX_WIDTH, aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT - 1);
	aphexWinInit(&winAscii, APHEX_WIN_ASCII_X, APHEX_WIN_ASCII_Y, APHEX_WIN_ASCII_WIDTH, aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT - 1);
	aphexWinInit(&winBin, APHEX_WIN_BIN_X, aphexTerm.ws_row - 1 - APHEX_WIN_BIN_HEIGHT - 1, aphexTerm.ws_col, APHEX_WIN_BIN_HEIGHT);

	aphexContentPrompt(&winBase);
	aphexContentBase(&winBase);
	aphexContentAddr(&winAddr);
	aphexContentHex(&winHex);
	aphexContentAscii(&winAscii);
	aphexContentBin(&winBin);

	aphexCursorSet(APHEX_WIN_HEX_X, APHEX_WIN_HEX_Y);
}

void aphexContentFree()
{
	free(winBase.c);
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
		snprintf(line, 11, "0x%.8X\n", i*16+buf.shiftOffset*16);
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
}

void aphexContentPrompt(aphexWin *win)
{
	char p[aphexTerm.ws_col-1];
	sprintf(p," aphex [%.3i %.3i] 0x%.8X nibble: %s shift: %.3i>\n",cursorX,cursorY,buf.offset,(buf.nibble?"HIGH":"LOW "),buf.shiftOffset);
	int i = 0;
	while (1) {
		if (p[i] == '\n') break;
		win->c[(win->height-1)*win->width + i] = p[i];
		i++;	
	}
	win->c[(win->height*win->width)-1] = '\n';
}

