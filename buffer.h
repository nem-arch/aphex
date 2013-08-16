#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

struct {
	char* fname;
	int f;	// filedescriptor for mmap
	unsigned char *mem;
	int memsize;
	int memindex;
	APHEX_NIBBLE nibble;
	APHEX_CMODE controlmode;
	APHEX_EDITMODE editmode;
} buf;

/* mmap file */
void buf_open(unsigned char* fname);

/* munmap */
void buf_close();

/* set memindex */
void buf_setindex(int index, APHEX_NIBBLE nibble);

/* edit mem at current index/nibble */
void buf_edit(unsigned char c);

#endif // BUFFER_H_INCLUDED
