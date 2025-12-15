#include "../include/kb.h"
#include "../include/print.h"
#include "../include/cstr.h"
#define KBD_DATA_PORT   0x60
#define KBD_STATUS_PORT 0x64

static volatile bool kb_running = true;

// Simple US QWERTY map for 0x02 - 0x0D, add more as needed
static char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', // 0x00-0x0D
    '\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', // 0x0E-0x1C
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\', // 0x1D-0x2A
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0  // 0x2B-0x3F
};

static char inb(uint16_t port) {
    char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Blocking function: wait for keypress and return str of length 1
str kb_getchar(char* buffer) {
    char c = 0;
    while (kb_running) {
        if (inb(KBD_STATUS_PORT) & 1) {
            unsigned char sc = inb(KBD_DATA_PORT);
            if (sc < 128) {
                c = scancode_map[sc];
                if (c) {
                    buffer[0] = c;
                    buffer[1] = '\0';
                    print_msg(buffer); // print char immediately
                    str s = { buffer, 1 };
                    return s;
                }
            }
        }
    }
    buffer[0] = '\0';
    str s = { buffer, 0 };
    return s;
}

// Reads until Enter (\n) or max_len reached, returns str
str kb_getline(char* buffer, int max_len) {
    str line;
    line.data = buffer;
    line.len = 0;

    while (line.len < (u32)(max_len - 1)) {
        char char_buf[2];
        str ch = kb_getchar(char_buf);
        if (ch.len == 0) continue; // skip if no char
        if (ch.data[0] == '\n') {
            line.data[line.len] = '\0';
            handle_next_line();
            return line;
        }
        line.data[line.len++] = ch.data[0];
    }

    line.data[line.len] = '\0';
    handle_next_line();
    return line;
}

// Stop kb_getchar loop
void kb_stop() {
    kb_running = false;
}

// Placeholder install function
void kb_install() {
    kb_running = true;
}
