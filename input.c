#include "types.h"

char getch()
{
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror ("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror ("tcsetattr ~ICANON");
	return (buf);
}

void aphexInputProcess()
{
	switch (getch()) {
		case ('j'):
			aphexCursorDown(1);
			break;
		case ('k'):
			aphexCursorDown(-1);
			break;
		case ('h'):
			aphexCursorRight(-1);
			break;
		case ('l'):
			aphexCursorRight(1);
			break;
		case ('q'):
			quit = true;
			break;
	}
}

void aphexCursorSet(int x, int y)
{
	if (cbxr(x) && cbxl(x) && cbyt(y) && cbyb(y)) {
		char tput[128];
		if ((x - APHEX_WIN_HEX_X)/3<8)
			sprintf(tput,"tput cup %i %i",y,x);
		if ((x - APHEX_WIN_HEX_X)/3>=8)
			sprintf(tput,"tput cup %i %i",y,x+1);
		system(tput);
		//cursorY = y;
		//cursorX = x;
	}
}

void aphexCursorDown(int y)
{
	if (y>0) {
		y += cursorY;
		if (!cbyb(y)) {
			// not in boundary
			if (buf_getoffset() + 16 < buf.memsize) {
				// in mem boundary
				buf.shiftOffset++;
			}
			return;
		} else {
			// in boundary
			if (buf_getoffset() + 16 > buf.memsize-1) {
				return;
			}
		}
		cursorY = y;
		return;
	}
	if (y<0) {
		y += cursorY;
		if (!cbyt(y)) {
			// not in boundary
			if (y < buf.shiftOffset*16) {
				// in mem boundary
				buf.shiftOffset--;
			}
			return;
		}
		cursorY = y;
		return;
	}
	// should never fall through here
}

bool cbyt(int y)
{
	return (y >= APHEX_WIN_HEX_Y);
}

bool cbyb(int y)
{
	return (y <= aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT - 1);
}

bool cbxl(int x)
{
	return (x >= APHEX_WIN_HEX_X);
}

bool cbxr(int x)
{
	return (x <= APHEX_WIN_HEX_X + APHEX_WIN_HEX_WIDTH );
}

void aphexCursorRight(int x)
{
	if (x>0) {
		x += cursorX;
		if (!cbxr(x+2)) {
			// not in boundary
			return;
		} else {
			// in boundary
			if (buf_getoffset() + (buf.nibble^APHEX_NIBBLE_HIGH) > buf.memsize-1) return;
			if ((x+1)%3) x++;
		}
		buf.nibble ^= APHEX_NIBBLE_HIGH;
		cursorX = x;
		return;
	}
	if (x<0) {
		x += cursorX;
		if (!cbxl(x)) {
			// not in boundary
			return;
		} else {
			// in boundary
			if ((x-1)%3) x--;
		}
		buf.nibble ^= APHEX_NIBBLE_HIGH;
		cursorX = x;
		return;
	}
	// should never fall through here
}

