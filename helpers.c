#include "types.h"

bool isHex(unsigned char c)
{
	return ( ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')) || isNum(c) );
}

bool isAlpha(unsigned char c)
{
	return ( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) );
}

bool isNum(unsigned char c)
{
	return ((c >= '0') && (c <= '9'));
}

bool isPrintable(unsigned char c)
{
	return ( ((c >= 0x20) && (c <= 0x7E)) );
}

unsigned char hexToNum(unsigned char c)
{
	if ((c >= 'a') && (c <= 'f')) {
		return (unsigned char)(c-'a'+0xA);
	}
	if ((c >= 'A') && (c <= 'F')) {
		return (unsigned char)(c-'A'+0xA);
	}
	if ( isNum(c) ) {
		return (unsigned char)(c-'0');
	}
	// should never fall through here, because isHex is checked before hexToNum
	return 0xFF;
}

char c2nH(char c)
{
	return ((c>>4)&0x0F);
}

char c2nL(char c)
{
	return (c&0x0F);
}

unsigned char c2a(unsigned char c)
{
	if ( isPrintable(c) ) {
		return (c);
	}
	return '.';
}

