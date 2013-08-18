#include "types.h"

void buf_open(unsigned char* fname)
{
	int openMode = O_RDONLY;
	int mmapMode = PROT_READ;
	if (aphexMode != APHEX_READONLY_MODE) {
		openMode = O_RDWR;
		mmapMode = PROT_READ|PROT_WRITE;
	}

	if ((buf.f = open((const char*)fname, openMode)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	struct stat sb;
	fstat(buf.f, &sb);

	/* check filesize */
	if (sb.st_size > ULONG_MAX) {
		printf("file too large (%lu MB, max: %lu MB)",sb.st_size/1024/1024,ULONG_MAX/1024/1024);
		close(buf.f);
		exit(EXIT_SUCCESS);
	}

	if ((buf.mem = (unsigned char*)mmap(NULL, sb.st_size, mmapMode, MAP_SHARED, buf.f, 0)) == MAP_FAILED) {
		close(buf.f);
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	buf.memsize = sb.st_size;
	buf.offset = 0;
	buf.shiftOffset = 0;
	buf.editmode = APHEX_HEX;
	buf.controlmode = APHEX_INSERT;
	buf.nibble = APHEX_NIBBLE_HIGH;
}

void buf_close()
{
	if (munmap(buf.mem, buf.memsize) < 0) {
		perror("munmap");
	}
	close(buf.f);
}

void buf_setoffset(int offset)
{
	buf.nibble = APHEX_NIBBLE_HIGH;
	if (offset < 0) {
		buf.offset = 0;
	}
	if ((offset >= 0) && (offset < buf.memsize - 1)) {
		buf.offset = offset;
	}
	if (offset >= buf.memsize) {
		buf.offset = buf.memsize-1;
	}
}

void buf_edit(unsigned char c)
{
	switch (aphexEditMode) {
		case (APHEX_EDIT_HEX):
			if (isHex(c)) {
				c = hexToNum(c);
				char cur = buf.mem[buf.offset];
				if (buf.nibble == APHEX_NIBBLE_HIGH) {
					buf.mem[buf.offset] = ((cur&0x0F)|(c<<4));
				}
				else {
					buf.mem[buf.offset] = ((cur&0xF0)|c);
				}
			}
			break;
		case (APHEX_EDIT_BIN):
			if ((c=='1')||(c=='0')) {
				buf.mem[buf.offset] = buf.mem[buf.offset]^(0x80>>(cursorX-APHEX_WIN_BIN_X));
			}
			break;
		case (APHEX_EDIT_ASCII):
			if (isPrintable(c)) {
				buf.mem[buf.offset] = c;
			}
			break;
	}
}

long buf_getoffset()
{
	return ((cursorX - APHEX_WIN_HEX_X)/3 + (cursorY - APHEX_WIN_HEX_Y)*16 + buf.shiftOffset*16);
}

void buf_memBin(char* byte)
{
	for (int i=0;i<4;i++) {
		byte[9-i] = ((buf.mem[buf.offset]&(1<<i))?'1':'0');
	}
	byte[4] = ' ';
	for (int i=0;i<4;i++) {
		byte[4-i] = ((buf.mem[buf.offset]&(1<<(i+4)))?'1':'0');
	}
}
