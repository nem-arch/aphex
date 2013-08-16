#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

typedef enum { true = 1, false = 0 } bool;
typedef enum { APHEX_NIBBLE_HIGH, APHEX_NIBBLE_LOW } APHEX_NIBBLE;
typedef enum { APHEX_HEX, APHEX_ASCII } APHEX_EDITMODE;
typedef enum { APHEX_INSERT, APHEX_COMMAND } APHEX_CMODE;

#include "buffer.h"
#include "view.h"
#include "input.h"
#include "helpers.h"

#endif // TYPES_H_INCLUDED
