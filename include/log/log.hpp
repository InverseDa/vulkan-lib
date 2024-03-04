#ifndef VULKAN_LIB_LOG_HPP
#define VULKAN_LIB_LOG_HPP

#include <iostream>
#include <cstdarg>
#include <format>

enum LOG_LEVEL {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

namespace IO {
inline void PrintLog(LOG_LEVEL level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    switch (level) {
    case LOG_LEVEL_INFO:
        std::cout << "[INFO] ";
        break;
    case LOG_LEVEL_WARNING:
        std::cout << "[WARNING] ";
        break;
    case LOG_LEVEL_ERROR:
        std::cout << "[ERROR] ";
        break;
    }
    std::vprintf(fmt, args);
    std::cout << std::endl;
    va_end(args);
}

inline void ThrowError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::vprintf(fmt, args);
    std::cout << std::endl;
    va_end(args);
    throw std::runtime_error("Error occurred");
}

} // namespace IO

#endif // VULKAN_LIB_LOG_HPP
