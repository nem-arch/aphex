#include "types.h"

bool isHex(unsigned char c)
{
	return ( ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')) || ((c >= '0') && (c <= '9')) );
}

unsigned char hexToNum(unsigned char c)
{
	if ((c >= 'a') && (c <= 'f')) {
		return (c-'a'+0xA);
	}
	if ((c >= 'A') && (c <= 'F')) {
		return (c-'A'+0xA);
	}
	if ((c >= '0') && (c <= '9')) {
		return (c-'0');
	}
	// should never fall through here, because isHex is checked before hexToNum
	return 0xFF;
}

void buf_open(unsigned char* fname)
{
	if ((buf.f = open((const char*)fname, O_RDWR)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	if ((buf.mem = (unsigned char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, buf.f, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	buf.memsize = 4096;
	buf_setindex(0, APHEX_NIBBLE_LOW);
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

void buf_setindex(int index, APHEX_NIBBLE nibble)
{
	buf.memindex = index;
	buf.nibble = nibble;
}

void buf_edit(unsigned char c)
{
	if (isHex(c)) {
		c = hexToNum(c);
		char cur = buf.mem[buf.memindex];
		if (buf.nibble == APHEX_NIBBLE_HIGH) {
			buf.mem[buf.memindex] = (cur&0x0F)|(c<<4);
		}
		else {
			buf.mem[buf.memindex] = (cur&0xF0)|c;
		}
	}
}
