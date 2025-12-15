#include "./include/print.h"
#include "./include/kb.h"
#include "./include/cstr.h"
void kernel_main() {
    clear_screen();
    print_msg("kernel_loaded!");
}
