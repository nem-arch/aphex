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
			aphexCursorSetYDown(comNum);
			resetComBuf();
			break;
		case (CURSOR_UP):
			aphexCursorSetYUp(comNum);
			resetComBuf();
			break;
		case (CURSOR_LEFT):
			aphexCursorSetXLeft(comNum);
			resetComBuf();
			break;
		case (CURSOR_RIGHT):
			aphexCursorSetXRight(comNum);
			resetComBuf();
			break;

		case (APHEX_INSERT_MODE):
			if (aphexMode != APHEX_READONLY_MODE) {
				aphexMode = APHEX_INSERT_MODE;
			}
			break;
		case (APHEX_COMMAND_MODE):
			if (aphexMode != APHEX_COMMAND_MODE) {
				aphexMode = APHEX_COMMAND_MODE; } break;
		case (APHEX_READONLY_MODE):
			if (aphexMode != APHEX_READONLY_MODE) {
				aphexMode = APHEX_READONLY_MODE;
			}
			break;
		case (APHEX_EDIT_HEX):
			break;
		case (APHEX_EDIT_ASCII):
			break;
		case (APHEX_EDIT_BIN):
			break;
		case (APHEX_DELETE_COMBUF):
			resetComBuf();
			break;
		case (QUIT):
			quit = true;
			resetComBuf();
			break;
		case (CURSOR_BOTTOM):
			resetComBuf();
			aphexCursorBottom();
			break;
		case (CURSOR_TOP):
			if (comIndex != 0) {
				aphexCursorSetByOffset(comNum);
				resetComBuf();
				break;
			}  else {
				if (comBuf[0] == 'g') {
					aphexCursorTop();
					resetComBuf();
					break;
				} else {
					comBuf[0] = 'g';
				}
			}
			break;
		case (CURSOR_HOME):
			aphexCursorHome();
			break;
		case (CURSOR_END):
			resetComBuf();
			aphexCursorEnd();
			break;
		case (APHEX_REDRAW):
			resetComBuf();
			aphexWinSetTermSize(&winBase);
			break;
		case (NONE):
			break;
	}
}

aphexCom parseComBuf(char c)
{
	if ((aphexMode == APHEX_COMMAND_MODE) || (aphexMode == APHEX_READONLY_MODE)) {
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
				return CURSOR_TOP;

				/* commands ignoring comBuf*/
			case ('i'):
				return APHEX_INSERT_MODE;
			case (0x1B):
				return APHEX_DELETE_COMBUF;
			case ('Q'):
				return QUIT;
			case ('G'):
				return CURSOR_BOTTOM;
			case ('$'):
				return CURSOR_END;
			case ('q'):
				return APHEX_REDRAW;
			case ('0'):
				if (comIndex == 0) {
					return CURSOR_HOME;
				}
			default:
				if (isNum(c)) {
					if ((comIndex>127) || (comNum + c > ULONG_MAX)) return NONE;
					comBuf[comIndex] = c;
					comNum = 0;
					int powTen = 1;
					for (int i=0; i<comIndex+1; i++) {
						comNum += (comBuf[comIndex - i]-'0')*powTen;
						powTen*=10;
					}
					comIndex++;
				}
				return NONE;
		}
	}
	if (aphexMode == APHEX_INSERT_MODE) {
		switch (c) {
			case (0x1B):
				return APHEX_COMMAND_MODE;
			case ('j'):
				return CURSOR_DOWN;
			case ('k'):
				return CURSOR_UP;
			case ('h'):
				return CURSOR_LEFT;
			case ('l'):
				return CURSOR_RIGHT;
			case ('Q'):
				return QUIT;
		}
		buf_edit(c);
	}
	return NONE;
}

void resetComBuf()
{
	comIndex = 0;
	comNum = 1;
	memset(comBuf,' ',128);
}

/* WINDOW UNDEPENDENT MOVEMENT */

void aphexCursorSetXRight(long o)
{
	if (o > 16 - (buf.offset%16)) {
		aphexCursorEnd();
	} else {
		if (o>1) o*=2;
		while (o>0) {
			aphexCursorRight(1);
			o--;
		}
	}
}

void aphexCursorSetXLeft(long o)
{
	if (o > ((buf.offset+1)%16)) {
		aphexCursorHome();
	} else {
		if (o>1) o*=2;
		while (o>0) {
			aphexCursorRight(-1);
			o--;
		}
	}
}

void aphexCursorSetYUp(long o)
{
	if (o > (buf.offset/16)) {
		aphexCursorTop();
	} else {
		while (o>0) {
			aphexCursorDown(-1);
			o--;
		}
	}
}

void aphexCursorSetYDown(long o)
{
	if (o > ((buf.memsize-1 - buf.offset)/16)) {
		aphexCursorBottom();
	} else {
		while (o>0) {
			aphexCursorDown(1);
			o--;
		}
	}
}

void aphexCursorHome()
{
	if ((buf.offset%16) == 0x0) return;
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
	if (buf.memsize/16 < (aphexWinMainBottom())) {
		// no shift needed, view shorter than term
		buf.shiftOffset = 0;
		buf.offset = buf.memsize - buf.memsize%16;
		cursorX = APHEX_WIN_HEX_X;
		cursorY = (buf.memsize/16) + APHEX_WIN_HEX_Y;
	} else {
		// shift needed, view longer than term
		buf.shiftOffset = buf.memsize/16 + (buf.memsize%16?1:0) - (aphexWinMainBottom()-1);
		buf.offset = buf.memsize - buf.memsize%16;
		cursorX = APHEX_WIN_HEX_X;
		cursorY = (aphexWinMainBottom()-1);
	}
}

void aphexCursorSetByOffset(long o)
{
	if (buf.nibble == APHEX_NIBBLE_LOW) {
		aphexCursorRight(-1);
	}
	long x = ((o%16) - (buf.offset%16));
	long y = ((o - buf.offset)/16);

	if (y>0) {
		aphexCursorSetYDown(y);
	} else {
		aphexCursorSetYUp(-y);
	}
	if (x>0) {
		aphexCursorSetXRight(x);
		if (y<0) aphexCursorDown(-1);
	} else {
		aphexCursorSetXLeft(-x);
		if (y>0) aphexCursorDown(1);
	}
	return;
}

/* WINDOW DEPENDENT MOVEMENT */

void aphexCursorSet(int x, int y)
{
	if (aphexCursorHexBorderXRight(x) && aphexCursorHexBorderXLeft(x) && aphexCursorHexBorderYTop(y) && aphexCursorHexBorderYBottom(y)) {
		if ((x - APHEX_WIN_HEX_X)/3<8)
			printf("\033[%i;%iH",y+1,x+1);
		if ((x - APHEX_WIN_HEX_X)/3>=8)
			printf("\033[%i;%iH",y+1,x+2);
		fflush(stdout);
	}
}

void aphexCursorDown(int y)
{
	if (y>0) {
		if (!aphexCursorHexBorderYBottom(y+cursorY)) {
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
		if (!aphexCursorHexBorderYTop(y+cursorY)) {
			// not in boundary
			if (buf_getoffset() + 16*y >= 0) {
				// in mem boundary
				buf.shiftOffset+=y;
				if (buf.shiftOffset<0) {
					// avoid negative shift and correct cursor
					cursorY += buf.shiftOffset;
					buf.shiftOffset = 0;
				}
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
		if (!aphexCursorHexBorderXRight(x+2+cursorX)) {
			// not in boundary
			return;
		} else {
			// in boundary
			if (buf_getoffset() + (buf.nibble^APHEX_NIBBLE_HIGH) > buf.memsize-1) return;
			if ((x+cursorX+2)%3) x++;
		}
		buf.nibble ^= APHEX_NIBBLE_HIGH;
		cursorX += x;
		buf.offset = buf_getoffset();
		return;
	}
	if (x<0) {
		if (!aphexCursorHexBorderXLeft(x+cursorX)) {
			// not in boundary
			return;
		} else {
			// in boundary
			if ((x+cursorX)%3) x--;
		}
		buf.nibble ^= APHEX_NIBBLE_HIGH;
		cursorX += x;
		buf.offset = buf_getoffset();
		return;
	}
	// should never fall through here
}

/* boundary check helpers */

bool aphexCursorHexBorderYTop(int y)
{
	return (y >= APHEX_WIN_HEX_Y);
}

bool aphexCursorHexBorderYBottom(int y)
{
	return (y < aphexWinMainBottom());
}

bool aphexCursorHexBorderXLeft(int x)
{
	return (x >= APHEX_WIN_HEX_X);
}

bool aphexCursorHexBorderXRight(int x)
{
	return (x <= APHEX_WIN_HEX_X + APHEX_WIN_HEX_WIDTH );
}

