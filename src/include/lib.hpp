#pragma once
#include <iostream>
#include <cstdint>
#include <sys/mman.h>

using i32 = int32_t;
using i64 = int64_t;
using u32 = uint32_t;
using u64 = uint64_t;

constexpr u64 KiB(u64 n) { return n << 10; }
constexpr u64 MiB(u64 n) { return n << 20; }
constexpr u64 GiB(u64 n) { return n << 30; }

constexpr u64 ALIGN(u64 n, u64 p) { return (n + (p - 1)) & ~(p - 1); }

constexpr int   PROT_RW   = PROT_READ | PROT_WRITE;
constexpr int   MAP_FLAGS = MAP_ANONYMOUS | MAP_PRIVATE;
constexpr u64   PAGE_SIZE = KiB(4);
constexpr u64   MINIMUM_SIZE = 4;