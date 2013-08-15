#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

struct winsize wnew, wold;

/* check windowsize and redraw if changed */
void win_checksize();

void win_redraw();

#endif // VIEW_H_INCLUDED
