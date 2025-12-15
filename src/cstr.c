#include "../include/cstr.h"

// Compute length of char* string
static u32 cstr_len(const char* str) {
    u32 len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// Create str from buffer and char*
str str_create(char* buffer, const char* src) {
    str s;
    s.data = buffer;
    u32 i = 0;
    while (src[i] != '\0') {
        s.data[i] = src[i];
        i++;
    }
    s.data[i] = '\0';
    s.len = i;
    return s;
}

// Copy str
void str_copy(str* dest, const str* src) {
    for (u32 i = 0; i < src->len; i++) {
        dest->data[i] = src->data[i];
    }
    dest->data[src->len] = '\0';
    dest->len = src->len;
}

// Concatenate str
void str_cat(str* dest, const str* src) {
    u32 i = 0;
    while (i < src->len) {
        dest->data[dest->len + i] = src->data[i];
        i++;
    }
    dest->len += src->len;
    dest->data[dest->len] = '\0';
}

// Compare str
int str_cmp(const str* s1, const str* s2) {
    u32 i = 0;
    while (i < s1->len && i < s2->len) {
        if (s1->data[i] != s2->data[i]) return s1->data[i] - s2->data[i];
        i++;
    }
    return (int)(s1->len - s2->len);
}

// Reverse str in-place
void str_reverse(str* s) {
    for (u32 i = 0; i < s->len / 2; i++) {
        char tmp = s->data[i];
        s->data[i] = s->data[s->len - 1 - i];
        s->data[s->len - 1 - i] = tmp;
    }
}

// Find char in str
int str_chr(const str* s, char ch) {
    for (u32 i = 0; i < s->len; i++) {
        if (s->data[i] == ch) return i;
    }
    return -1;
}

// Integer to str
str str_itoa(char* buffer, int value) {
    int i = 0;
    int negative = 0;

    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        str s = { buffer, 1 };
        return s;
    }

    if (value < 0) {
        negative = 1;
        value = -value;
    }

    while (value != 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (negative) buffer[i++] = '-';
    buffer[i] = '\0';

    // Reverse buffer
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char tmp = buffer[j];
        buffer[j] = buffer[k];
        buffer[k] = tmp;
    }

    str s = { buffer, (u32)i };
    return s;
}

// str to integer
int str_atoi(const str* s) {
    int res = 0;
    u32 i = 0;
    int negative = 0;

    if (s->len > 0 && s->data[0] == '-') {
        negative = 1;
        i = 1;
    }

    while (i < s->len) {
        res = res * 10 + (s->data[i] - '0');
        i++;
    }

    if (negative) res = -res;
    return res;
}

// Substring
str str_substring(str* s, u32 start, u32 length, char* dest_buffer) {
    str sub;
    sub.data = dest_buffer;
    sub.len = 0;

    if (start >= s->len) {
        sub.data[0] = '\0';
        return sub;
    }

    u32 end = start + length;
    if (end > s->len) end = s->len;

    u32 j = 0;
    for (u32 i = start; i < end; i++) {
        sub.data[j++] = s->data[i];
    }
    sub.len = j;
    sub.data[j] = '\0';
    return sub;
}
