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

void process_input()
{
	switch (getch()) {
		case ('j'):
			cursor_move(0,1);
			break;
		case ('k'):
			cursor_move(0,-1);
			break;
		case ('h'):
			cursor_move(-1,0);
			break;
		case ('l'):
			cursor_move(1,0);
			break;
		case ('q'):
			quit = true;
			break;
	}
}

void cursor_set(int x, int y)
{
	if ((x <= aphexTerm.ws_col-1) && (x >= 0) && (y <= aphexTerm.ws_row-1) && (y >= 0)) {
		char tput[128];
		sprintf(tput,"tput cup %i %i",y,x);
		system(tput);
		cursorX = x;
		cursorY = y;
	}
}

void cursor_move(int x, int y)
{
	x += cursorX;
	y += cursorY;
	cursor_set(x,y);
}

