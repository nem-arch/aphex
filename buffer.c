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
