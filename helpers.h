#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

/* is char hexadecimal, alphabetic, number, printable */
bool isHex(unsigned char c);
bool isAlpha(unsigned char c);
bool isNum(unsigned char c);
bool isPrintable(unsigned char c);

/* char to number */
unsigned char hexToNum(unsigned char);

/* returns high nibble of char for hex view */
char c2nH(char c);

/* returns low nibble of char for hex view */
char c2nL(char c);

/* returns char if printable, else '.' for ascii view */
unsigned char c2a(unsigned char c);

#endif // HELPERS_H_INCLUDED
