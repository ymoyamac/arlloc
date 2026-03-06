#pragma once

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <sys/mman.h>
#include <string>

using i32 = int32_t;
using i64 = int64_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usize = std::size_t;

constexpr u64 KiB(u64 n) { return n << 10; }
constexpr u64 MiB(u64 n) { return n << 20; }
constexpr u64 GiB(u64 n) { return n << 30; }

constexpr usize ALIGNMENT = sizeof(void*);
constexpr u64  ALIGN(u64 n)  { return (n + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1); }
constexpr u64  PAGE_SIZE  = KiB(4);
constexpr int  PROT_RW    = PROT_READ | PROT_WRITE;
constexpr int  MAP_FLAGS  = MAP_ANONYMOUS | MAP_PRIVATE;
constexpr u64  MINIMUM_SIZE = 4;

template<typename T, std::size_t N>
constexpr std::size_t len(T (&)[N]) { return N; }

class Logger {
private:
    static const inline std::string INFO  = "\x1B[32m[INFO]:\033[0m\t";
    static const inline std::string WARN  = "\x1B[33m[WARN]:\033[0m\t";
    static const inline std::string ERROR = "\x1B[91m[ERROR]:\033[0m\t";

    // Formatea el mensaje igual que printf
    static std::string format(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        
        // Calcular tamaño necesario
        int size = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);

        std::string result(size + 1, '\0');
        
        va_start(args, fmt);
        vsnprintf(result.data(), size + 1, fmt, args);
        va_end(args);

        result.resize(size); // quitar el '\0' final
        return result;
    }

public:
    static void divider() {
        printf("%s ============================================================================================================\n", INFO.c_str());
    }

    static void info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        int size = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);

        std::string msg(size + 1, '\0');

        va_start(args, fmt);
        vsnprintf(msg.data(), size + 1, fmt, args);
        va_end(args);

        msg.resize(size);
        printf("%s %s\n", INFO.c_str(), msg.c_str());
    }

    static void warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        int size = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);

        std::string msg(size + 1, '\0');

        va_start(args, fmt);
        vsnprintf(msg.data(), size + 1, fmt, args);
        va_end(args);

        msg.resize(size);
        printf("%s %s\n", WARN.c_str(), msg.c_str());
    }

    static void error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);

        int size = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);

        std::string msg(size + 1, '\0');

        va_start(args, fmt);
        vsnprintf(msg.data(), size + 1, fmt, args);
        va_end(args);

        msg.resize(size);
        printf("%s %s\n", ERROR.c_str(), msg.c_str());
    }
};