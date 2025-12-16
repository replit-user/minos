# 🧱 MINOS: OS Dev Friendly Template

MINOS (Minimal Operating System) is a bare-bones, C-based template designed for aspiring operating system developers. It provides essential, low-level APIs for I/O and a simple disk-based filesystem, allowing you to quickly bootstrap your kernel development.

---

## 💻 Included APIs

MINOS provides four core utility modules:

### 1. `print.c` (VGA Text Output)

This module handles **VGA text mode** output for displaying messages to the screen. It directly interacts with the VGA text buffer located at physical address `0xb8000`. 

| Function | Description |
| :--- | :--- |
| `clear_screen(void)` | Clears the entire VGA screen buffer. |
| `handle_next_line(void)` | Moves the cursor to the beginning of the next line, handling screen wrapping (or clearing the rest of the current line in this implementation). |
| `print_msg(char* msg)` | Prints a null-terminated string to the VGA screen. Supports `\n` for a new line. |

**Key Details:**
* Each character on the screen occupies 2 bytes (16 bits) in the buffer: 8 bits for the **ASCII character** and 8 bits for the **color/attributes**.
* The current color attribute is hardcoded to `15` (bright white foreground on black background).

---

### 2. `kb.c` (Keyboard Input)

A simple, blocking keyboard input module that reads from the PS/2 keyboard data port (`0x60`).

| Function | Description |
| :--- | :--- |
| `kb_getchar(char* buffer)` | **Blocking** call. Waits for a keypress, prints the character immediately to the screen, and returns a `str` of length 1 containing the character. |
| `kb_getline(char* buffer, int max_len)` | **Blocking** call. Reads characters until the user presses **Enter (`\n`)** or `max_len` is reached. Returns the input as a `str`. |
| `kb_install()` | Placeholder function to initialize the keyboard state (`kb_running = true`). |
| `kb_stop()` | Stops the `kb_getchar` loop by setting `kb_running = false`. |

**Key Details:**
* Uses a simple **US QWERTY scancode map** for basic characters.
* The `inb` function is an inline assembly wrapper to read a byte from a specified I/O port, typically used for hardware communication.

---

### 3. `fs.c` (Minimal Filesystem)

This module implements a basic, flat-file filesystem suitable for a small OS. It operates on a conceptual `fs_disk` structure which requires external `read_block` and `write_block` implementations (not shown here) for persistent storage.

| Function | Description |
| :--- | :--- |
| `fs_init(fs_disk* disk)` | Sets the active disk pointer for all subsequent filesystem operations. |
| `fs_format(fs_disk* disk)` | Initializes the disk with a fresh filesystem structure: **Superblock**, **Inode Table**, and **Bitmap**. |
| `fs_create_file(fs_disk* disk, const str* name)` | Creates a new file entry in the Inode Table with the given name. Returns `0` on success. |
| `fs_write_file(fs_disk* disk, const str* name, const void* data, u32 size)` | Writes `size` bytes of data to the named file, allocating new data blocks as needed. Returns the number of bytes written. |
| `fs_read_file(fs_disk* disk, const str* name, void* out, u32 max_size)` | Reads file content into the `out` buffer, up to `max_size`. Returns the number of bytes read. |
| `fs_list_files(fs_list_cb callback)` | Iterates over all used inodes and calls the provided callback function for each file found. |

**Filesystem Layout:** 
1.  **Block 0: Superblock:** Stores metadata like magic number, block size, and total blocks.
2.  **Block 1: Inode Table:** Stores `fs_inode` structures (name, size, type, data block pointers).
3.  **Block 2: Bitmap:** Tracks which data blocks are currently allocated/free.
4.  **Block 3+: Data Blocks:** The actual storage area for file contents.

---

### 4. `cstr.c` (String Utilities)

A custom C string library for handling character arrays (`char*`) and the custom `str` structure (which holds a pointer to the data and its length).

| Function | Description |
| :--- | :--- |
| `str_len(const char* str)` | Computes the length of a null-terminated C string. |
| `str_create(char* buffer, const char* src)` | Creates a new `str` structure by copying a C string into a buffer. |
| `str_copy(str* dest, const str* src)` | Copies one `str` to another. |
| `str_cat(str* dest, const str* src)` | Concatenates two `str` structures. |
| `str_cmp(const str* s1, const str* s2)` | Compares two `str` structures. |
| `str_reverse(str* s)` | Reverses an `str` in-place. |
| `str_chr(const str* s, char ch)` | Finds the first occurrence of a character in an `str`. |
| `str_itoa(char* buffer, int value)` | Converts an integer to an `str`. |
| `str_atoi(const str* s)` | Converts an `str` to an integer. |
| `str_substring(str* s, u32 start, u32 length, char* dest_buffer)` | Extracts a substring from an `str`. |

---

## 🛠️ Getting Started

1.  **Integrate:** Include the necessary headers (`print.h`, `kb.h`, `fs.h`, `cstr.h`, etc.) into your kernel environment.
2.  **Initialize:** Set up the VGA output by using `clear_screen()`.
3.  **Use:** Start printing messages using `print_msg()` and handle user input with `kb_getline()`.
4.  **Filesystem:** Implement the low-level `read_block` and `write_block` functions for your chosen storage medium (e.g., hard drive, floppy, or in-memory array) and initialize the filesystem with `fs_init()` and `fs_format()`.

This template provides the foundational building blocks for screen I/O, keyboard interaction, and persistent storage management, enabling you to focus on the higher-level kernel and application design.
