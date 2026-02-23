#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdint.h>

typedef int32_t i32;
typedef int64_t i64;
typedef uint32_t u32;
typedef uint64_t u64;

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define ALIGN(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

#define PROT_RW PROT_READ | PROT_WRITE
#define MAP_FLAGS MAP_ANONYMOUS | MAP_PRIVATE

#define PAGE_SIZE KiB(4)
#define len(arr) sizeof(arr)/sizeof(arr[0])

#endif