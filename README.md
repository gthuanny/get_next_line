# get_next_line

*This project has been created as part of the 42 curriculum by gde-cast.*

## Description

get_next_line is a C function that reads a file line by line. Each time you call it, you get the next line from a file descriptor until the end of the file is reached.

The main challenge is handling different buffer sizes while managing memory efficiently and maintaining state between function calls using static variables.

**What you learn:**
- File I/O with read() system call
- Static variables and memory persistence
- Dynamic memory allocation and management
- Buffer handling and string manipulation

## Instructions

### Compilation

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

The BUFFER_SIZE can be any positive value:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=1 *.c      # small buffer
cc -Wall -Wextra -Werror -D BUFFER_SIZE=4096 *.c   # large buffer
```

### Usage Example

```c
#include "get_next_line.h"
#include <fcntl.h>

int main(void)
{
    int fd = open("file.txt", O_RDONLY);
    char *line;

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

### Testing

```bash
# Create test file
echo -e "Line 1\nLine 2\nLine 3" > test.txt

# Compile and run
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 *.c && ./a.out

# Check for memory leaks
valgrind --leak-check=full ./a.out
```

## Algorithm Explanation

The algorithm follows a three-phase approach for each function call:

### 1. Read Phase (read_file)
Reads from the file descriptor in chunks of BUFFER_SIZE bytes until it finds a newline character or reaches EOF. The data is accumulated in a buffer that persists between calls.

**Why chunks?** Reading byte-by-byte would require too many system calls. Reading in larger chunks (BUFFER_SIZE) is much more efficient.

### 2. Extract Phase (copy_line)
Takes the accumulated buffer and extracts exactly one line (up to and including the `\n` character). This line is returned to the caller.

### 3. Store Phase (get_remainder)
Saves everything after the extracted line back into the static variable for the next function call. This is what makes reading line-by-line possible.

### The Key: Static Variables

```c
static char *text;  // persists between function calls
```

Without `static`, the variable would be destroyed when the function returns, and we'd lose all unprocessed data. With `static`, the variable remembers leftover content between calls.

**Example:**
```
File: "Hello\nWorld\n"

Call 1: Read "Hello\nWorld\n" → Return "Hello\n" → Store "World\n"
Call 2: Use stored "World\n"  → Return "World\n" → Store NULL
Call 3: Nothing stored        → Return NULL (end of file)
```

### Why This Algorithm?

**Alternatives considered:**
- Reading byte-by-byte: Too slow (too many system calls)
- Reading entire file: Doesn't work for large files or stdin
- Fixed-size line buffer: Won't work for lines longer than buffer

**This solution:**
- Works with any buffer size (tested with 1 to 10000000+)
- Handles lines of any length
- Minimal memory usage (only stores what's needed)
- Efficient (minimal system calls)

### Edge Cases Handled
- Empty files
- Files without final newline
- Multiple consecutive newlines
- Lines longer than BUFFER_SIZE
- Invalid file descriptors
- Memory allocation failures

## Project Structure

```
get_next_line/
├── get_next_line.c       # main logic (4 functions)
├── get_next_line_utils.c # helper functions (strlen, strchr, strjoin)
└── get_next_line.h       # prototypes and defines
```

**Functions:**
- `get_next_line()` - orchestrates everything
- `read_file()` - accumulates data until finding a line
- `copy_line()` - extracts one line from accumulated data
- `get_remainder()` - stores leftover data for next call
- `ft_strlen()`, `ft_strchr()`, `ft_strjoin()` - utility functions

## Resources

### Documentation
- [read(2) man page](https://man7.org/linux/man-pages/man2/read.2.html)
- [open(2) man page](https://man7.org/linux/man-pages/man2/open.2.html)
- [File I/O in C Tutorial](https://www.tutorialspoint.com/cprogramming/c_file_io.htm)
- [Static Variables in C](https://www.geeksforgeeks.org/static-variables-in-c/)

### Testing Tools
- [Tripouille/gnlTester](https://github.com/Tripouille/gnlTester)
- [francinette](https://github.com/xicodomingues/francinette)

### AI Usage

AI was used as a learning aid to help understand concepts like static variables and file descriptors through explanations and examples. All code implementation and problem-solving was done independently.

---

*42 Porto*
