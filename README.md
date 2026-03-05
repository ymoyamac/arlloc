# arlloc

A custom memory allocator written in C++, implementing an **arena allocator** strategy as an alternative to the system's `malloc`/`free`.

## The Problem

Requesting memory from the operating system is not the hard part — `mmap` handles that. The real challenge is **managing the memory that `mmap` gives us**: knowing where each region and block lives, tracking which blocks are free, and handing out pointers to the right locations without overlapping or losing memory.

`arlloc` solves this by maintaining a structured layout inside each memory page, using metadata headers to track every allocation and its state.

## Overview

`arlloc` manages memory through two levels of abstraction: **Regions** and **Blocks**. Memory is requested from the OS via `mmap` and distributed internally using a bump allocator strategy combined with a free block list, making allocations fast while keeping track of every byte.

## Architecture

### Arlloc

`Arlloc` is the top-level coordinator. It maintains a list of all active regions and a list of free blocks available for reuse. When the user requests memory, `Arlloc` first searches the free block list. If no suitable block is found, it creates a new region.

```
Arlloc
├── regions:     Region1 → Region2 → ...
└── free_blocks: Block(64) → Block(128) → ...
```

### Region

A Region is a contiguous block of memory obtained directly from the OS via a single `mmap` call. Each region is one page in size (4096 bytes), with 56 bytes reserved for metadata and 4040 bytes available as an allocatable buffer.

The Region struct is constructed **in-place at the start of the mmap page** using placement new, so no additional allocation is needed.

```
mmap page (4096 bytes)
+---------------------+------------------------------------------+
|  Region (56 bytes)  |        buffer (4040 bytes)               |
+---------------------+------------------------------------------+
^                     ^
*region               *buffer = region + sizeof(Region)
```

Blocks are laid out sequentially inside the buffer as allocations are made:

```
buffer
+----------+------------------+----------+------------------+------+
|  Block   |   user data      |  Block   |   user data      | ...  |
| (header) |   (size bytes)   | (header) |   (size bytes)   |      |
+----------+------------------+----------+------------------+------+
^                              ^
offset (before)                offset (after)
```

When a region runs out of space, `Arlloc` creates a new one and links it to the list.

### Block

A Block represents an individual allocation inside a Region. Each block has a 32-byte metadata header placed immediately before the user data pointer that is returned to the caller.

```
32 bytes (metadata)           X bytes (user data)
+--------------------------------------------------------------------+
| is_free    (1 byte)  |                                            |
| size       (8 bytes) |              Allocatable Buffer            |
| region     (8 bytes) |                                            |
| user_data  (8 bytes) |                                            |
+--------------------------------------------------------------------+
^                      ^
*block                 *user_data  ← this pointer is returned to the user
```

## How it works

### Allocation

1. `Arlloc::alloc(size)` first searches `free_blocks` for a block large enough to fit the request.
2. If a suitable free block is found, **block splitting** is performed: the free block is divided into a used block of exactly `size` bytes and a new smaller free block with the remaining space.
3. If no suitable free block exists, a new Region is created via `mmap` and a new Block is placed at the current offset.

Memory is aligned to 8-byte boundaries using the `ALIGN` macro, ensuring safe access across architectures.

```cpp
// Request memory from the allocator
void* ptr = arlloc.alloc(64);

// Free a specific block
arlloc.dealloc(ptr);
```

### Deallocation

`dealloc(void* ptr)` recovers the Block header by stepping back `sizeof(Block)` bytes from the user pointer, marks the block as free, and adds it to the free block list for future reuse. Memory is not returned to the OS until the allocator is destroyed.

```
ptr
+-----------+------------------+
|   Block   |   user data      |
| (header)  |                  |
+-----------+------------------+
^           ^
block       ptr
```

### Destruction

When `Arlloc` is destroyed, its destructor iterates all regions and calls `Region::drop` on each one, which manually invokes the Region destructor and releases the mmap page via `munmap`.

## Trade-offs

| Advantage | Limitation |
|---|---|
| O(1) push/pop at head and tail | Per-block free requires scanning free list |
| Block splitting minimizes wasted space | Coalescing of adjacent free blocks not yet implemented |
| No fragmentation within a single allocation | Region size is fixed to PAGE_SIZE |
| Cache-friendly sequential layout within regions | Not suitable for long-lived mixed-size allocations |

## Use cases

This pattern is well suited for:

- Game engines with frame-based allocation cycles
- Compilers and parsers that allocate during a single pass
- Embedded systems with predictable memory lifetimes
- Any context requiring many small, fast allocations with a well-defined lifecycle