#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

/* is char hexadecimal */
bool isHex(unsigned char c);

bool isNum(unsigned char c);

/* char to number */
unsigned char hexToNum(unsigned char);

/* returns high nibble of char for hex view */
char c2nH(char c);

/* returns low nibble of char for hex view */
char c2nL(char c);

/* returns char if drawable, else '.' for ascii view */
char c2a(char c);

#endif // HELPERS_H_INCLUDED
