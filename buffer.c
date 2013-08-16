#include "types.h"

void buf_open(unsigned char* fname)
{
	if ((buf.f = open((const char*)fname, O_RDWR)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	struct stat sb;
	fstat(buf.f, &sb);
	if ((buf.mem = (unsigned char*)mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, buf.f, 0)) == MAP_FAILED) {
		close(buf.f);
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	buf.memsize = sb.st_size;
	buf_setoffset(0, APHEX_NIBBLE_HIGH);
	buf.shiftOffset = 0;
	buf.editmode = APHEX_HEX;
	buf.controlmode = APHEX_INSERT;
}

void buf_close()
{
	if (munmap(buf.mem, buf.memsize) < 0) {
		perror("munmap");
	}
	close(buf.f);
}

void buf_setoffset(int offset, APHEX_NIBBLE nibble)
{
	if (offset < 0) {
		//buf.offset = 0;
		//buf.nibble = APHEX_NIBBLE_HIGH;
	}
	if ((offset >= 0) && (offset < buf.memsize)) {
		buf.offset = offset;
		buf.nibble = nibble;
	}
	if (offset >= buf.memsize) {
		//buf.offset = buf.memsize;
		//buf.nibble = APHEX_NIBBLE_LOW;
	}
}

void buf_edit(unsigned char c)
{
	if (isHex(c)) {
		c = hexToNum(c);
		char cur = buf.mem[buf.offset];
		if (buf.nibble == APHEX_NIBBLE_HIGH) {
			buf.mem[buf.offset] = (cur&0x0F)|(c<<4);
		}
		else {
			buf.mem[buf.offset] = (cur&0xF0)|c;
		}
	}
}

int buf_getoffset()
{
	return ((cursorX - APHEX_WIN_HEX_X)/3 + (cursorY - APHEX_WIN_HEX_Y)*16 + buf.shiftOffset*16);
}
