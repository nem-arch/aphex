#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

typedef struct _aphexBuf {
	char* fname;
	int f;	// filedescriptor for mmap
	unsigned char *mem;
	long memsize;
	long offset;
	long shiftOffset;
	APHEX_NIBBLE nibble;
	APHEX_CMODE controlmode;
	APHEX_EDITMODE editmode;
} aphexBuf;

aphexBuf buf;

/* mmap file */
void buf_open(unsigned char* fname);

/* munmap */
void buf_close();

/* set buf.offset */
void buf_setoffset(int offset);

/* edit mem at current index/nibble */
void buf_edit(unsigned char c);

/* DEPRECATED - return current buf.offset */
long buf_getoffset();

/* write 9 bytes back to *byte (2 nibbles, 1 space) */
void buf_memBin(char *byte);

#endif // BUFFER_H_INCLUDED
