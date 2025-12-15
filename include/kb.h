#ifndef KB_H
#define KB_H

#include "cstr.h"

// Custom types
typedef unsigned char u8;
typedef unsigned int  u32;
typedef int          i32;
typedef char         bool;

#define true  1
#define false 0

// Initialize keyboard
void kb_install();

// Stop keyboard input loop
void kb_stop();

// Blocking function: wait for a keypress and return str of length 1
// buffer must be at least 2 chars
str kb_getchar(char* buffer);

// Reads until Enter (\n) or max_len reached, returns str
// buffer must be at least max_len chars
str kb_getline(char* buffer, int max_len);

#endif
