# Utils

## Description

This is a small self-written library with implementations for
common miscellaneous programming tasks.
I wrote it so that I don't have to reimplement the same functions
over and over again in my C projects.

## Dependencies

- [RND\_ErrMsg](https://github.com/randoragon/rnd-libs/tree/master/errmsg)

## Installation

Run the following code, if necessary as root:

```sh
make install
```

The header file will be copied to `/usr/include/` and the library file to `/usr/lib/`.

## Usage

Make sure to `#include <RND_BitArray.h>` and link with `librnd_bitarray.so` (this is usually
done by passing the `-lrnd_bitarray` flag to the compiler).

Note that this library is statically compiled, because its function definitions are
very small.

## Examples

See `example.c` file for basic usage. Refer to the source code for a complete list of functions
and other details.

