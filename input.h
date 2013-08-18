#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

typedef enum { APHEX_EDIT_ASCII = 'A',
	APHEX_EDIT_BIN = 'B',
	APHEX_COMMAND_MODE = 'C',
	APHEX_EDIT_HEX = 'H',
	APHEX_INSERT_MODE = 'I',
	APHEX_READONLY_MODE = 'R',
	CURSOR_UP, CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT, CURSOR_BOTTOM, CURSOR_TOP, CURSOR_HOME, CURSOR_END,
	APHEX_REDRAW, APHEX_DELETE_COMBUF,
	APHEX_CYCLE_EDIT_MODE, QUIT, NONE } aphexCom;

char comBuf[128];
unsigned long comNum;
int comIndex;
void resetComBuf();

aphexCom parseComBuf(char c);
aphexCom aphexMode;
aphexCom aphexEditMode;

bool quit;

char getch();

int cursorX, cursorY;
void aphexCursorSet(int x, int y);
void aphexCursorDown(int y);
void aphexCursorRight(int x);
void aphexCursorTop();
void aphexCursorBottom();
void aphexCursorHome();
void aphexCursorEnd();
void aphexCursorSetByOffset(long o);

void aphexCursorSetXRight(long o);
void aphexCursorSetXLeft(long o);
void aphexCursorSetYUp(long o);
void aphexCursorSetYDown(long o);

/* catch user input, redraw window, set cursor */
void aphexInputProcess();

void aphexCycleEditMode();

bool aphexCursorBorderYTop(int y);
bool aphexCursorBorderYBottom(int y);
bool aphexCursorBorderXLeft(int x);
bool aphexCursorBorderXRight(int x);

#endif // INPUT_H_INCLUDED
