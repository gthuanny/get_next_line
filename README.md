# get_next_line

*This project has been created as part of the 42 curriculum by gde-cast.*

## Description

get_next_line is a function that reads and returns a single line from a file descriptor. The primary goal is to implement a robust file reading mechanism that handles variable buffer sizes while managing memory efficiently. This project teaches fundamental concepts including static variables, dynamic memory allocation, and file I/O operations in C.

The function must work with any valid file descriptor (files, standard input, etc.) and any buffer size, returning one line at a time until the end of the file is reached. Each call to `get_next_line()` returns the next line including the newline character `\n`, or NULL when the file ends or an error occurs.

**Key learning objectives:**
- Understanding and implementing static variables for state persistence
- Mastering dynamic memory management with malloc/free
- Working with file descriptors and the read() system call
- Handling edge cases and buffer management
- Writing modular, reusable code

## Instructions

### Compilation

The project must be compiled with the following flags:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

The `BUFFER_SIZE` macro defines how many bytes are read at once. It can be set to any positive value during compilation:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=1 get_next_line.c get_next_line_utils.c main.c
cc -Wall -Wextra -Werror -D BUFFER_SIZE=4096 get_next_line.c get_next_line_utils.c main.c
```

### Files Structure

```
get_next_line/
├── get_next_line.c       # Main function and core logic
├── get_next_line_utils.c # Helper functions (strlen, strchr, strjoin)
├── get_next_line.h       # Header file with prototypes and defines
└── README.md             # This file
```

### Basic Usage

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int fd;
    char *line;

    fd = open("test.txt", O_RDONLY);
    if (fd == -1)
        return (1);
    
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);  // Don't forget to free!
    }
    
    close(fd);
    return (0);
}
```

### Testing

Create a test file:
```bash
echo -e "First line\nSecond line\nThird line" > test.txt
```

Compile and run:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
./a.out
```

Expected output:
```
First line
Second line
Third line
```

### Memory Leak Testing

Always test with valgrind to ensure no memory leaks:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./a.out
```

The output should show:
```
All heap blocks were freed -- no leaks are possible
```

## Algorithm Explanation and Justification

### Overall Strategy

The algorithm uses a **buffered reading approach with persistent state** to efficiently read files line by line. The core strategy relies on three fundamental concepts:

1. **Static variable for state persistence**: A static variable stores leftover data between function calls, eliminating the need to re-read already processed content.

2. **Buffer-based reading**: Instead of reading one byte at a time (which would require excessive system calls), data is read in chunks defined by BUFFER_SIZE.

3. **Three-phase processing**: Each call follows a read-extract-store cycle.

### Detailed Algorithm Breakdown

#### Phase 1: Reading (read_file)

**Purpose**: Accumulate enough data to contain at least one complete line.

**Process**:
1. Check if the static buffer already contains a newline character
2. If not, allocate a temporary buffer of BUFFER_SIZE
3. Read from the file descriptor into this buffer
4. Concatenate the new data with existing data
5. Repeat until a newline is found or EOF is reached

**Justification**: This approach minimizes system calls while handling variable-length lines. Reading in chunks is significantly faster than byte-by-byte reading (one system call for 4096 bytes vs 4096 system calls for individual bytes).

```
Example with BUFFER_SIZE=5:
File contains: "Hello\nWorld\n"

Call 1: read "Hello" → no \n found → continue
Call 2: read "\nWorl" → \n found → stop
Result: "Hello\nWorl" accumulated
```

#### Phase 2: Extraction (copy_line)

**Purpose**: Extract exactly one line from the accumulated text.

**Process**:
1. Count characters until the first `\n` or end of string
2. Allocate memory for the line (length + `\n` + `\0`)
3. Copy characters up to and including the `\n`
4. Add null terminator

**Justification**: This creates an independent copy of the line that can be returned to the caller. The original accumulated text remains intact for the next phase.

```
Input: "Hello\nWorld\n"
Output: "Hello\n"
```

#### Phase 3: Storage (get_remainder)

**Purpose**: Preserve unprocessed data for the next function call.

**Process**:
1. Find the position after the first newline
2. If no content remains, free everything and return NULL
3. Otherwise, allocate memory for the remainder
4. Copy everything after the newline
5. Free the old buffer
6. Return the new buffer (stored in static variable)

**Justification**: This ensures no data is lost between calls. The static variable retains this remainder, allowing the next call to continue from where it left off without re-reading the file.

```
Input: "Hello\nWorld\n"
After extracting "Hello\n"
Remainder stored: "World\n"
```

### Why Static Variables?

The use of a static variable is crucial to this algorithm:

**Without static**:
- Function local variables are destroyed when the function returns
- Each call would start from the beginning of the file
- Previously read data would be lost

**With static**:
- The variable persists between function calls
- Leftover data from previous reads is preserved
- File reading continues from the exact position where it stopped

```c
// First call
static char *text = NULL;
// After processing: text = "World\n" (stored)

// Second call
// text still contains "World\n" (persisted!)
```

### Buffer Size Trade-offs

The algorithm's efficiency depends heavily on BUFFER_SIZE:

**Small buffer (e.g., BUFFER_SIZE=1)**:
- Pros: Minimal memory usage
- Cons: Many system calls → slower execution
- Use case: Memory-constrained environments

**Large buffer (e.g., BUFFER_SIZE=4096)**:
- Pros: Fewer system calls → faster execution
- Cons: More memory usage
- Use case: Performance-critical applications

**Testing**: The algorithm must work correctly with ANY buffer size (1, 42, 10000000, etc.), demonstrating its robustness.

### Edge Cases Handled

1. **Empty file**: Returns NULL immediately
2. **File without final newline**: Returns the last line without `\n`
3. **Multiple consecutive newlines**: Each treated as a separate line
4. **Very long lines**: Handles lines larger than BUFFER_SIZE by reading multiple chunks
5. **Invalid file descriptor**: Returns NULL
6. **Memory allocation failure**: Returns NULL after cleanup

### Time Complexity

- **Best case**: O(n) where n is the file size (each byte read once)
- **Worst case**: O(n) (same - data is never re-read)
- **Per call**: O(k) where k is the line length

### Space Complexity

- O(BUFFER_SIZE + remaining_text_length)
- Static variable grows/shrinks dynamically based on content

### Alternative Approaches Considered

**1. Reading byte-by-byte**:
- Rejected: Would require n system calls for an n-byte file (extremely inefficient)

**2. Reading entire file at once**:
- Rejected: Doesn't scale for large files; wastes memory

**3. Using a global buffer array**:
- Rejected: Doesn't handle multiple file descriptors in the bonus version

The chosen algorithm strikes the optimal balance between memory efficiency and performance.

## Features

- ✅ Reads from any valid file descriptor
- ✅ Handles variable BUFFER_SIZE (compile-time configurable)
- ✅ Memory-efficient (only stores necessary data)
- ✅ Properly frees all allocated memory
- ✅ Returns NULL on EOF or error
- ✅ Includes newline character in returned line
- ✅ Norm compliant (42 coding standards)

## Technical Choices

### Memory Management Strategy

**ft_strjoin design decision**: The function automatically frees the first parameter (s1) after concatenation. This design choice simplifies the main logic by avoiding manual free operations in the read loop.

```c
// Instead of:
temp = text;
text = ft_strjoin(text, buff);
free(temp);

// We can simply write:
text = ft_strjoin(text, buff);  // ft_strjoin frees old text
```

### Error Handling

All functions return NULL on error and ensure proper cleanup:
- malloc() failures immediately free any allocated memory
- read() errors trigger cleanup of both buffer and accumulated text
- Invalid parameters (fd < 0, BUFFER_SIZE <= 0) are caught early

## Common Issues and Solutions

### Problem: Segmentation Fault

**Cause**: Accessing unallocated memory or dereferencing NULL pointers

**Solution**: 
- Always check if malloc() succeeded before using the pointer
- Verify pointers aren't NULL before accessing their content

### Problem: Memory Leaks

**Cause**: Missing free() calls or losing pointer references

**Solution**:
- Every malloc() must have a corresponding free()
- Use valgrind to track all allocations
- Be careful when reassigning pointers

### Problem: Wrong Output with Different BUFFER_SIZE

**Cause**: Algorithm assumes specific buffer size

**Solution**: The algorithm must be buffer-size agnostic. Test with values like 1, 42, 10000.

## Resources

### Official Documentation
- [Linux man pages - read(2)](https://man7.org/linux/man-pages/man2/read.2.html) - Understanding the read() system call
- [Linux man pages - open(2)](https://man7.org/linux/man-pages/man2/open.2.html) - File descriptor operations
- [C Dynamic Memory](https://en.cppreference.com/w/c/memory) - malloc() and free() reference

### Tutorials and Articles
- [File I/O in C](https://www.tutorialspoint.com/cprogramming/c_file_io.htm) - Basic file operations in C
- [Static Variables in C](https://www.geeksforgeeks.org/static-variables-in-c/) - Understanding static keyword
- [Valgrind Quick Start](https://valgrind.org/docs/manual/quick-start.html) - Memory leak detection

### Testing Tools
- [Tripouille/gnlTester](https://github.com/Tripouille/gnlTester) - Comprehensive tester for get_next_line
- [xicodomingues/francinette](https://github.com/xicodomingues/francinette) - 42 project tester suite

### AI Usage

AI tools (Claude) were used for the following tasks:

**Documentation**:
- Generating this README structure and improving clarity of explanations
- Creating detailed algorithm explanations with examples
- Proofreading and ensuring technical accuracy

**Learning Resources**:
- Explaining complex concepts (static variables, file descriptors, buffer management)
- Providing visual examples and analogies for better understanding
- Generating edge case scenarios to consider

**NOT used for**:
- Writing the actual implementation code (get_next_line.c, get_next_line_utils.c)
- Solving algorithmic challenges
- Debugging the code

The core logic, algorithm design, and implementation were developed independently through research, testing, and problem-solving.

## Project Structure and Function Overview

### get_next_line.c

**get_next_line(int fd)**
- Main function that orchestrates the entire process
- Validates input parameters
- Coordinates read-extract-store cycle
- Returns one line per call

**read_file(int fd, char *text)**
- Reads from file descriptor until at least one complete line exists
- Manages the buffer and accumulates data
- Handles read errors and EOF

**copy_line(char *text)**
- Extracts a single line from accumulated text
- Allocates and returns independent line buffer
- Includes the `\n` character if present

**get_remainder(char *text)**
- Preserves unprocessed data for next call
- Stores result in static variable
- Frees old buffer to prevent leaks

### get_next_line_utils.c

**ft_strlen(const char *str)**
- Calculates string length
- Handles NULL input safely

**ft_strchr(char *s, int c)**
- Searches for character in string
- Critical for detecting newline characters

**ft_strjoin(char *s1, char *s2)**
- Concatenates two strings
- Automatically frees s1 (design choice)
- Returns newly allocated combined string

---

*Made at 42 Lisboa*
