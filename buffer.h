#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

typedef struct _aphexBuf {
	char* fname;
	int f;	// filedescriptor for mmap
	unsigned char *mem;
	int memsize;
	int offset;
	int shiftOffset;
	APHEX_NIBBLE nibble;
	APHEX_CMODE controlmode;
	APHEX_EDITMODE editmode;
} aphexBuf;

aphexBuf buf;

/* mmap file */
void buf_open(unsigned char* fname);

/* munmap */
void buf_close();

/* set memindex */
void buf_setoffset(int offset, APHEX_NIBBLE nibble);

/* edit mem at current index/nibble */
void buf_edit(unsigned char c);

int buf_getoffset();

#endif // BUFFER_H_INCLUDED
