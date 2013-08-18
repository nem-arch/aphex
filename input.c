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
		case (APHEX_CYCLE_EDIT_MODE):
			aphexCycleEditMode();
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
			case (0x09):
				return APHEX_CYCLE_EDIT_MODE;
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
		if (aphexEditMode != APHEX_EDIT_ASCII) {
			switch (c) {
				case (0x1B):
					return APHEX_COMMAND_MODE;
				case (0x09):
					return APHEX_CYCLE_EDIT_MODE;
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
		} else {
			if (c==0x1B) {
				return APHEX_COMMAND_MODE;
			}
			if (c==0x09) {
				return APHEX_CYCLE_EDIT_MODE;
			}
			if (c=='Q') {
				return QUIT;
			}
			if ((c==0x7F)&&(aphexEditForward)) {
				return CURSOR_LEFT;
			}
		}
		buf_edit(c);
		if ((aphexEditMode == APHEX_EDIT_ASCII)&&(aphexEditForward)) {
			return CURSOR_RIGHT;
		}
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
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			aphexCursorRight(1);
			break;
		case (APHEX_EDIT_HEX):
			if (o > 15 - (buf.offset%16) + 1 - (buf.nibble^APHEX_NIBBLE_HIGH)) {
				if ((aphexMode == APHEX_INSERT_MODE)&&(aphexEditForward)) {
					aphexCursorHome();
					aphexCursorDown(1);
				}
			} else {
				if (o>1) o*=2;
				while (o>0) {
					aphexCursorRight(1);
					o--;
				}
			}
			break;
		case (APHEX_EDIT_ASCII):
			if (o > 15 - (buf.offset%16)) {
				if ((aphexMode == APHEX_INSERT_MODE)&&(aphexEditForward)) {
					aphexCursorHome();
					aphexCursorDown(1);
				}
			} else {
				while (o>0) {
					aphexCursorRight(1);
					o--;
				}
			}
			break;
	}
}

void aphexCursorSetXLeft(long o)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			aphexCursorRight(-1);
			break;
		case (APHEX_EDIT_HEX):
			if (o > ((buf.offset)%16) + (buf.nibble^APHEX_NIBBLE_HIGH)) {
				if ((aphexMode == APHEX_INSERT_MODE)&&(aphexEditForward)) {
					aphexCursorDown(-1);
					aphexCursorEnd();
				}
			} else {
				if (o>1) o*=2;
				while (o>0) {
					aphexCursorRight(-1);
					o--;
				}
			}
			break;
		case (APHEX_EDIT_ASCII):
			if (o > ((buf.offset)%16)) {
				if ((aphexMode == APHEX_INSERT_MODE)&&(aphexEditForward)) {
					aphexCursorDown(-1);
					aphexCursorEnd();
				}
			} else {
				while (o>0) {
					aphexCursorRight(-1);
					o--;
				}
			}
			break;
	}
}

void aphexCursorSetYUp(long o)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return;
		case (APHEX_EDIT_HEX):
		case (APHEX_EDIT_ASCII):
			if (o > (buf.offset/16)) {
				aphexCursorTop();
			} else {
				while (o>0) {
					aphexCursorDown(-1);
					o--;
				}
			}
			break;
	}
}

void aphexCursorSetYDown(long o)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return;
		case (APHEX_EDIT_HEX):
		case (APHEX_EDIT_ASCII):
			if (o > ((buf.memsize-1 - buf.offset)/16)) {
				aphexCursorBottom();
			} else {
				while (o>0) {
					aphexCursorDown(1);
					o--;
				}
			}
	}
}

void aphexCursorHome()
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			cursorX = APHEX_WIN_BIN_X;
			return;
		case (APHEX_EDIT_HEX):
			cursorX = APHEX_WIN_HEX_X;
			break;
		case (APHEX_EDIT_ASCII):
			cursorX = APHEX_WIN_ASCII_X;
			break;
	}
	if ((buf.offset%16) == 0x0) return;
	buf.nibble = APHEX_NIBBLE_HIGH;
	buf.offset &= 0xFFFFFFF0;
}

void aphexCursorEnd()
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return;
		case (APHEX_EDIT_HEX):
			if (buf.offset + 16 < buf.memsize) {
				if ((buf.offset&0x0000001F) == 0x1F) return;
				// full line
				buf.offset |= 0xF;
				cursorX = APHEX_WIN_HEX_X + APHEX_WIN_HEX_WIDTH - 2;
			} else {
				if ((buf.offset == buf.memsize-1) || ( (buf.offset&0x0000001F) == 0x1F) ) return;
				// nonfull line
				cursorX = APHEX_WIN_HEX_X + ((buf.memsize-1)%16)*3 +1;
				buf.offset = (buf.memsize-(buf.memsize%16)) + ((buf.memsize-1)%16);
			}
			break;
		case (APHEX_EDIT_ASCII):
			if (buf.offset + 16 < buf.memsize) {
				if ((buf.offset&0x0000001F) == 0x1F) return;
				// full line
				cursorX = APHEX_WIN_ASCII_X + APHEX_WIN_ASCII_WIDTH - 2;
				buf.offset |= 0xF;
			} else {
				if ((buf.offset == buf.memsize-1) || ((buf.offset&0x0000001F) == 0x1F)) return;
				// nonfull line
				cursorX = APHEX_WIN_ASCII_X + ((buf.memsize-1)%16);
				buf.offset = (buf.memsize-(buf.memsize%16)) + ((buf.memsize-1)%16);
			}
			break;
	}
	buf.nibble = APHEX_NIBBLE_LOW;
}

void aphexCursorTop()
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return;
		case (APHEX_EDIT_HEX):
			cursorX = APHEX_WIN_HEX_X;
			cursorY = APHEX_WIN_HEX_Y;
			break;
		case (APHEX_EDIT_ASCII):
			cursorX = APHEX_WIN_ASCII_X;
			cursorY = APHEX_WIN_ASCII_Y;
			break;
	}
	buf.shiftOffset = 0;
	buf.nibble = APHEX_NIBBLE_HIGH;
	buf.offset = 0;
}

void aphexCursorBottom()
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return;
		case (APHEX_EDIT_HEX):
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
			break;
		case (APHEX_EDIT_ASCII):
			if (buf.memsize/16 < (aphexWinMainBottom())) {
				// no shift needed, view shorter than term
				buf.shiftOffset = 0;
				buf.offset = buf.memsize - buf.memsize%16;
				cursorX = APHEX_WIN_ASCII_X;
				cursorY = (buf.memsize/16) + APHEX_WIN_ASCII_Y;
			} else {
				// shift needed, view longer than term
				buf.shiftOffset = buf.memsize/16 + (buf.memsize%16?1:0) - (aphexWinMainBottom()-1);
				buf.offset = buf.memsize - buf.memsize%16;
				cursorX = APHEX_WIN_ASCII_X;
				cursorY = (aphexWinMainBottom()-1);
			}
			break;
	}
	buf.nibble = APHEX_NIBBLE_HIGH;
}

void aphexCursorSetByOffset(long o)
{
	if (aphexEditMode == APHEX_EDIT_BIN) return;
	if (buf.nibble == APHEX_NIBBLE_LOW) {
		aphexCursorRight(-1);
	}
	long x = ((o%16) - (buf.offset%16));
	long y = ((o/16) - (buf.offset/16));

	if (y>0) {
		aphexCursorSetYDown(y);
	} else {
		aphexCursorSetYUp(-y);
	}
	if (x>0) {
		aphexCursorSetXRight(x);
	} else {
		aphexCursorSetXLeft(-x);
	}
	return;
}

/* WINDOW DEPENDENT MOVEMENT */

void aphexCursorSet(int x, int y)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			if (aphexCursorBorderXRight(x) && aphexCursorBorderXLeft(x) && aphexCursorBorderYTop(y) && aphexCursorBorderYBottom(y)) {
				if ((x - APHEX_WIN_BIN_X)<4)
					printf("\033[%i;%iH",y+1,x+1);
				if ((x - APHEX_WIN_BIN_X)>=4)
					printf("\033[%i;%iH",y+1,x+2);
				fflush(stdout);
			}
			break;
		case (APHEX_EDIT_HEX):
			if (aphexCursorBorderXRight(x) && aphexCursorBorderXLeft(x) && aphexCursorBorderYTop(y) && aphexCursorBorderYBottom(y)) {
				if ((x - APHEX_WIN_HEX_X)/3<8)
					printf("\033[%i;%iH",y+1,x+1);
				if ((x - APHEX_WIN_HEX_X)/3>=8)
					printf("\033[%i;%iH",y+1,x+2);
				fflush(stdout);
			}
			break;
		case (APHEX_EDIT_ASCII):
			if (aphexCursorBorderXRight(x) && aphexCursorBorderXLeft(x) && aphexCursorBorderYTop(y) && aphexCursorBorderYBottom(y)) {
				if ((x - APHEX_WIN_ASCII_X)<8)
					printf("\033[%i;%iH",y+1,x+1);
				if ((x - APHEX_WIN_ASCII_X)>=8)
					printf("\033[%i;%iH",y+1,x+2);
				fflush(stdout);
			}
			break;
	}
}

void aphexCursorDown(int y)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			break;
		case (APHEX_EDIT_HEX):
		case (APHEX_EDIT_ASCII):
			if (y>0) {
				if (!aphexCursorBorderYBottom(y+cursorY)) {
					// not in boundary
					if (buf.offset + 16*y < buf.memsize) {
						// in mem boundary
						buf.shiftOffset+=y;
						buf.offset += y*16;
					}
					return;
				} else {
					// in boundary
					if (buf.offset + 16*y > buf.memsize-1) {
						// out of mem boundary
						return;
					}
				}
				cursorY += y;
				buf.offset += y*16;
				return;
			}
			if (y<0) {
				if (!aphexCursorBorderYTop(y+cursorY)) {
					// not in boundary
					if (buf.offset + 16*y >= 0) {
						// in mem boundary
						buf.shiftOffset+=y;
						if (buf.shiftOffset<0) {
							// avoid negative shift and correct cursor
							cursorY += buf.shiftOffset;
							buf.shiftOffset = 0;
						}
						buf.offset += y*16;
					}
					return;
				} else {
					// in boundary
					if (buf.offset + 16*y < 0) {
						// out of mem boundary
						return;
					}
				}
				cursorY += y;
				buf.offset += y*16;
				return;
			}
			break;
	}
	// should never fall through here
}

void aphexCursorRight(int x)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			if (x>0) {
				if (cursorX + x + 2> APHEX_WIN_BIN_X + 9) {
					// not in boundary
					return;
				}
			} else {
				if (x<0) {
					if (!aphexCursorBorderXLeft(x+cursorX)) {
						//not in boundary
						return;
					}
				}
			}
			cursorX += x;
			if (cursorX - APHEX_WIN_BIN_X < 4) {
				buf.nibble = APHEX_NIBBLE_HIGH;
			} else {
				buf.nibble = APHEX_NIBBLE_LOW;
			}
			break;
		case (APHEX_EDIT_HEX):
			if (x>0) {
				if (!aphexCursorBorderXRight(x+2+cursorX)) {
					// not in boundary
					return;
				} else {
					// in boundary
					if (buf.offset + (buf.nibble^APHEX_NIBBLE_HIGH) > buf.memsize-1) return;
					if ((x+cursorX+2)%3) x++;
				}
			} else {
				if (x<0) {
					if (!aphexCursorBorderXLeft(x+cursorX)) {
						// not in boundary
						return;
					} else {
						// in boundary
						if (buf.offset + (buf.nibble^APHEX_NIBBLE_HIGH) == 0) return;
						if ((x+cursorX)%3) x--;
					}
				}
			}
			cursorX += x;
			buf.nibble ^= APHEX_NIBBLE_HIGH;
			buf.offset = buf_getoffset();
			break;
		case (APHEX_EDIT_ASCII):
			if (x>0) {
				if (!aphexCursorBorderXRight(x+2+cursorX)) {
					// not in boundary
					return;
				} else {
					// in boundary
					if (buf.offset > buf.memsize-2) return;
				}
			} else {
				if (x<0) {
					if (!aphexCursorBorderXLeft(x+cursorX)) {
						// not in boundary
						return;
					} else {
						// in boundary
						if (buf.offset == 0) return;
					}
				}
			}
			cursorX += x;
			buf.offset += x;
			break;
	}
}

void aphexCycleEditMode()
{
	int x = buf.offset%16;
	int y = buf.offset/16 - buf.shiftOffset + 1;
	int o = (buf.offset/16)*16;
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			aphexEditMode = APHEX_EDIT_HEX;
			// get the line offset and just walk there
			cursorX = APHEX_WIN_HEX_X;
			cursorY = y;
			x*=2;
			x+=(buf.nibble^1);
			buf.nibble = APHEX_NIBBLE_HIGH;
			buf.offset = o;
			while (x>0) {
				aphexCursorRight(1);
				x--;
			}
			break;
		case (APHEX_EDIT_HEX):
			aphexEditMode = APHEX_EDIT_ASCII;
			// get the line offset and just walk there
			cursorX = APHEX_WIN_ASCII_X;
			buf.offset = o;
			while (x>0) {
				aphexCursorRight(1);
				x--;
			}
			break;
		case (APHEX_EDIT_ASCII):
			aphexEditMode = APHEX_EDIT_BIN;
			cursorX = APHEX_WIN_BIN_X + (buf.nibble^1)*4;
			cursorY = aphexWinMainBottom() + 1;
			break;
	}
}

/* boundary check helpers */

bool aphexCursorBorderYTop(int y)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return (y >= aphexWinMainBottom());
		case (APHEX_EDIT_HEX):
			return (y >= APHEX_WIN_HEX_Y);
		case (APHEX_EDIT_ASCII):
			return (y >= APHEX_WIN_ASCII_Y);
	}
	return false;
}

bool aphexCursorBorderYBottom(int y)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return (y < aphexWinMainBottom() + APHEX_WIN_BIN_HEIGHT);
		case (APHEX_EDIT_HEX):
			return (y < aphexWinMainBottom());
		case (APHEX_EDIT_ASCII):
			return (y < aphexWinMainBottom());
	}
	return false;
}

bool aphexCursorBorderXLeft(int x)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return (x >= APHEX_WIN_BIN_X);
		case (APHEX_EDIT_HEX):
			return (x >= APHEX_WIN_HEX_X);
		case (APHEX_EDIT_ASCII):
			return (x >= APHEX_WIN_ASCII_X);
	}
	return false;
}

bool aphexCursorBorderXRight(int x)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_BIN):
			return (x <= APHEX_WIN_BIN_X + APHEX_WIN_BIN_WIDTH);
		case (APHEX_EDIT_HEX):
			return (x <= APHEX_WIN_HEX_X + APHEX_WIN_HEX_WIDTH);
		case (APHEX_EDIT_ASCII):
			return (x <= APHEX_WIN_ASCII_X + APHEX_WIN_ASCII_WIDTH);
	}
	return false;
}

