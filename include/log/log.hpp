#ifndef VULKAN_LIB_LOG_HPP
#define VULKAN_LIB_LOG_HPP

#include <iostream>
#include <format>

enum LOG_LEVEL {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
};

namespace IO {
template <typename... Args>
inline void PrintLog(LOG_LEVEL level, const std::format_string<Args...>& fmt, Args&&... args) {
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
    std::cout << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

template <typename... Args>
inline void ThrowError(const std::format_string<Args...>& fmt, Args&&... args) {
    auto msg = std::format(fmt, std::forward<Args>(args)...);
    msg = "[ERROR] " + msg;
    std::cout << msg << std::endl;
    throw std::runtime_error(msg);
}

} // namespace IO

#endif // VULKAN_LIB_LOG_HPP
