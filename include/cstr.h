#ifndef CSTR_H
#define CSTR_H

typedef unsigned int uint16_t;
typedef unsigned int u32;

// Custom string type
typedef struct {
    char* data;  // pointer to char buffer
    u32 len;     // length of string (excluding null terminator)
} str;
u32 str_len(const char* str);
// Create a str from a char* (assumes buffer is large enough)
str str_create(char* buffer, const char* src);

// Copy a str
void str_copy(str* dest, const str* src);

// Concatenate two str objects (dest must have enough space)
void str_cat(str* dest, const str* src);

// Compare two str objects
int str_cmp(const str* s1, const str* s2);

// Reverse a str in-place
void str_reverse(str* s);

// Find character in str, returns index or -1
int str_chr(const str* s, char ch);

// Convert integer to str
str str_itoa(char* buffer, int value);

// Convert str to integer
int str_atoi(const str* s);

// Get substring (dest buffer must be large enough)
str str_substring(str* s, u32 start, u32 length, char* dest_buffer);
#endif
