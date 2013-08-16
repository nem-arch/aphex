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
	switch (parseComBuf(getch())) {
		case (CURSOR_DOWN):
			aphexCursorDown(comNum);
			comBuf[0] = '\0';
			comNum = 1;
			break;
		case (CURSOR_UP):
			aphexCursorDown(-comNum);
			comBuf[0] = '\0';
			comNum = 1;
			break;
		case (CURSOR_LEFT):
			aphexCursorRight(-comNum);
			comBuf[0] = '\0';
			comNum = 1;
			break;
		case (CURSOR_RIGHT):
			aphexCursorRight(comNum);
			comBuf[0] = '\0';
			comNum = 1;
			break;
		case (QUIT):
			quit = true;
			break;
		case (CURSOR_TOP):
			aphexCursorTop();
			break;
		case (CURSOR_BOTTOM):
			aphexCursorBottom();
			break;
		case (CURSOR_HOME):
			aphexCursorHome();
			break;
		case (CURSOR_END):
			aphexCursorEnd();
			break;
		case (NONE):
			break;
	}
}

aphexCom parseComBuf(char c)
{
	comNum = 1;
	switch (c) {
		/* commands working with comBuf */
		case ('j'):
			return CURSOR_DOWN;
		case ('k'):
			return CURSOR_UP;
		case ('h'):
			return CURSOR_LEFT;
		case ('l'):
			return CURSOR_RIGHT;
		case ('g'):
			return CURSOR_BOTTOM;

		/* commands ignoring comBuf*/
		case ('q'):
			comBuf[0] = '\0';
			return QUIT;
		case ('G'):
			comBuf[0] = '\0';
			return CURSOR_TOP;
		case ('0'):
			if (comBuf[0]!='\0') return NONE;
			comBuf[0] = '\0';
			return CURSOR_HOME;
		case ('$'):
			comBuf[0] = '\0';
			return CURSOR_END;
		default:
			if (isNum(c)) {
				sprintf(comBuf,"%s%c\n",comBuf,c);
				comNum = (c-'0');
			}
			return NONE;
	}
	return NONE;
}

void aphexCursorHome()
{
	if ((buf.offset&0x000000F) == 0x0) return;
	buf.nibble = APHEX_NIBBLE_HIGH;
	buf.offset &= 0xFFFFFFF0;
	cursorX = APHEX_WIN_HEX_X;
}

void aphexCursorEnd()
{
	buf.nibble = APHEX_NIBBLE_LOW;
	if (buf.offset + 16 < buf.memsize) {
		if ((buf.offset&0x0000001F) == 0x1F) return;
		// full line
		buf.offset |= 0xF;
		cursorX = APHEX_WIN_HEX_X + APHEX_WIN_HEX_WIDTH -2;
	} else {
		if ((buf.offset == buf.memsize-1) || ((buf.offset&0x0000001F) == 0x1F)) return;
		// nonfull line
		cursorX = APHEX_WIN_HEX_X + (buf.memsize-buf.offset-1)*3 +1;
		buf.offset = (buf.offset/16)*16 + (buf.memsize-buf.offset-1);;
	}
}

void aphexCursorTop()
{
	buf.shiftOffset = 0;
	buf.nibble = APHEX_NIBBLE_HIGH;
	buf.offset = 0;
	cursorX = APHEX_WIN_HEX_X;
	cursorY = APHEX_WIN_HEX_Y;
}

void aphexCursorBottom()
{
	buf.nibble = APHEX_NIBBLE_HIGH;
	if (buf.memsize/16 < (aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT -1)) {
		buf.shiftOffset = 0;
		buf.offset = buf.memsize - buf.memsize%16;
		cursorX = APHEX_WIN_HEX_X;
		cursorY = (buf.memsize/16) + APHEX_WIN_HEX_Y;
	} else {
		buf.shiftOffset = buf.memsize/16 + (buf.memsize%16?1:0) - (aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT - 1);
		buf.offset = buf.memsize - buf.memsize%16;
		cursorX = APHEX_WIN_HEX_X;
		cursorY = (aphexTerm.ws_row - 3 - APHEX_WIN_BIN_HEIGHT - 1);
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
		if (!cbyb(y+cursorY)) {
			// not in boundary
			if (buf_getoffset() + 16*y < buf.memsize) {
				// in mem boundary
				buf.shiftOffset+=y;
				buf.offset = buf_getoffset();
			}
			return;
		} else {
			// in boundary
			if (buf_getoffset() + 16*y > buf.memsize-1) {
				// out of mem boundary
				return;
			}
		}
		cursorY += y;
		buf.offset = buf_getoffset();
		return;
	}
	if (y<0) {
		if (!cbyt(y+cursorY)) {
			// not in boundary
			if (buf_getoffset() + 16*y >= 0) {
				// in mem boundary
				buf.shiftOffset+=y;
				buf.offset = buf_getoffset();
			}
			return;
		} else {
			// in boundary
			if (buf_getoffset() + 16*y < 0) {
				// out of mem boundary
				return;
			}
		}
		cursorY += y;
		buf.offset = buf_getoffset();
		return;
	}
	// should never fall through here
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
		buf.offset = buf_getoffset();
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
		buf.offset = buf_getoffset();
		return;
	}
	// should never fall through here
}

/* boundary check helpers */

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

