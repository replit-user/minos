#include "./include/print.h"
#include "./include/kb.h"
#include "./include/cstr.h"
#include "./include/fs.h"
void reset_buffer_manual(char* buffer, unsigned int size) {
    unsigned int i;
    for (i = 0; i < size; ++i) {
        buffer[i] = '\0'; // Writing the null terminator (0) to each byte
    }
}
/* simple string equality (no libc) */
static int streq(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == b[i];
}
void print_file(const str* name, u32 size, u8 type) {
    print_msg(name->data);
    print_msg("\n");
}
void kernel_main() {
    clear_screen();
}

