#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

struct {
	int origin;
	int lineLenAddr;
	int lineLenHex;
	int lineLenColumn;
	WINDOW *lineAddrWin;
	WINDOW *lineHexWin;
	WINDOW *lineAsciiWin;
	WINDOW *cmdWin;
	PANEL *lineAddrPanel;
	PANEL *lineHexPanel;
	PANEL *lineAsciiPanel;
	PANEL *promptPanel;
} view;

void view_open(int lineLenAddr, int lineLenHex, int lineLenColumn);

void view_close();

void view_clear();

void view_prompt();

void view_cursor();

void view_update();

void view_display();

#endif // VIEW_H_INCLUDED
