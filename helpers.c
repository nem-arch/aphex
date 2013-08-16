#include "types.h"

bool isHex(unsigned char c)
{
	return ( ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')) || isNum(c) );
}

bool isNum(unsigned char c)
{
	return ((c >= '0') && (c <= '9'));
}

unsigned char hexToNum(unsigned char c)
{
	if ((c >= 'a') && (c <= 'f')) {
		return (unsigned char)(c-'a'+0xA);
	}
	if ((c >= 'A') && (c <= 'F')) {
		return (unsigned char)(c-'A'+0xA);
	}
	if ((c >= '0') && (c <= '9')) {
		return (unsigned char)(c-'0');
	}
	// should never fall through here, because isHex is checked before hexToNum
	return 0xFF;
}

unsigned char c2nH(unsigned char c)
{
	return (c>>4);
}

unsigned char c2nL(unsigned char c)
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

