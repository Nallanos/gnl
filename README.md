*This project has been created as part of the 42 curriculum by abenameu.*

# get_next_line

## Description

`get_next_line` is a C function that reads a file descriptor **one line at a
time**. Each call returns the next line, including its terminating `\n` (except
for the last line of a file that does not end with one), and returns `NULL`
once there is nothing left to read or when an error occurs.

```c
char	*get_next_line(int fd);
```

It works with regular files, standard input and pipes, and with any
`BUFFER_SIZE` given at compile time. The goal of the project is to learn how
**static variables** keep data between function calls, and how to manage
memory carefully when reading data in chunks of arbitrary size.

## Instructions

The function is made of three files:

| File | Content |
|---|---|
| `get_next_line.c` | `get_next_line`, the read loop and the buffer handling |
| `get_next_line_utils.c` | string helpers (`ft_strlen`, `ft_strchr`, `ft_strdup`, `ft_strcat`, `ft_strswap`) |
| `get_next_line.h` | prototypes and the default `BUFFER_SIZE` (42) |

Compile them with your own `main`, optionally choosing the buffer size:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 main.c get_next_line.c get_next_line_utils.c
```

Example `main`:

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("file.txt", O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

Every returned line is allocated with `malloc` and must be freed by the caller.

## Algorithm

The function keeps a **static buffer** of `BUFFER_SIZE + 1` bytes. Between two
calls, this buffer holds the *remainder*: the bytes that were already read from
the file but belong to the next lines.

Each call works in four steps:

1. **Start from the remainder.** `line` is initialised with a copy of what is
   left in the buffer (an empty string on the first call, since a static array
   is zero-initialised).
2. **Read until a newline.** While `line` contains no `\n` and `read` still
   returns data, read `BUFFER_SIZE` bytes into the buffer, terminate it with
   `\0`, and append it to `line` up to and including the first `\n`.
3. **Shift the remainder.** Everything after the first `\n` in the buffer is
   copied to the beginning of the buffer (`rotate_buff`). If there is no `\n`,
   the buffer is emptied.
4. **Detect the end of file.** If `line` is empty, nothing was left to read: it
   is freed and `NULL` is returned.

Example with `BUFFER_SIZE = 4` and the file `"abcdef\ngh"`:

```
call 1: line = ""            read "abcd" -> line = "abcd"
                             read "ef\ng" -> line = "abcdef\n"   (stop: \n found)
        shift: buffer = "g"                                  -> returns "abcdef\n"
call 2: line = "g"           read "h"    -> line = "gh"
                             read ""     -> end of file
        shift: buffer = ""                                   -> returns "gh"
call 3: line = ""            read ""     -> line is empty    -> returns NULL
```

**Why this approach:**

- **Shifting the remainder to the start of the buffer** means the function
  never needs to remember a position: the unprocessed data is always at index
  0. Simply moving a pointer would not work, because a pointer parameter is a
  local copy and an array cannot be reassigned, and reading at an offset would
  overflow the buffer.
- **Using a fixed-size static array** instead of a static `malloc`'d string
  means nothing is left allocated when the function returns `NULL`: the only
  memory to free is the returned line.
- **Reading only until the first `\n`** keeps each call as cheap as possible
  and makes the function work with any `BUFFER_SIZE`, from 1 to very large
  values.

Errors (`fd < 0`, `BUFFER_SIZE <= 0`, failing `read` or `malloc`) return `NULL`
without leaking memory.

## Resources

- `man 2 read`, `man 3 malloc`, `man 3 free`
- [Static variables in C](https://en.cppreference.com/w/c/language/storage_duration)
- [File descriptors](https://en.wikipedia.org/wiki/File_descriptor)
- The 42 Norm (norminette) and peer discussions

### Use of AI

An AI assistant (Claude) was used as a reviewer during this project:

- explaining why the remainder was lost between calls (a pointer parameter is
  a local copy, and an array cannot be reassigned) and why the remainder must
  be shifted to the start of the static buffer;
- pointing out bugs in my code (uninitialised pointers, missing `\0` after
  `read`, uninitialised `malloc` memory before `ft_strcat`, off-by-one in
  `ft_strlen`) and explaining their cause;
- writing a test script and a small test `main`, which are not part of the
  submission;
- reformatting the code to pass `norminette`.

The algorithm and the code were written by me; every suggested fix was
understood and applied before being kept.
