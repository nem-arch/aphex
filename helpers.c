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

char c2nH(char c)
{
	return (c>>4);
}

char c2nL(char c)
{
	return (c&0xF);
}

char c2a(char c)
{
	if ((c >= 0x20) && (c <= 0x7E)) {
		return (c);
	}
	return '.';
}

