#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

typedef enum { CURSOR_UP, CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT, CURSOR_BOTTOM, CURSOR_TOP, CURSOR_HOME, CURSOR_END,
	APHEX_REDRAW, APHEX_INSERT_MODE = 'I', APHEX_COMMAND_MODE = 'C', APHEX_READONLY_MODE = 'R', APHEX_DELETE_COMBUF,  QUIT, NONE } aphexCom;

char comBuf[128];
int comNum;
aphexCom parseComBuf(char c);
aphexCom aphexMode;

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

/* catch user input, redraw window, set cursor */
void aphexInputProcess();

bool cbyt(int y);
bool cbyb(int y);
bool cbxl(int x);
bool cbxr(int x);

#endif // INPUT_H_INCLUDED
