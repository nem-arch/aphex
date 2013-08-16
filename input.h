#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

bool quit;

char getch();

int cursorX, cursorY;
void aphexCursorSet(int x, int y);
void aphexCursorDown(int y);
void aphexCursorRight(int x);

/* catch user input, redraw window, set cursor */
void aphexInputProcess();

bool cbyt(int y);
bool cbyb(int y);
bool cbxl(int x);
bool cbxr(int x);

#endif // INPUT_H_INCLUDED
