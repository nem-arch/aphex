#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

bool quit;

char getch();

int cursorX, cursorY;
void cursor_set(int x, int y);
void cursor_move(int x, int y);

/* catch user input, redraw window, set cursor */
void process_input();

#endif // INPUT_H_INCLUDED
