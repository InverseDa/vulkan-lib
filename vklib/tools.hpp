#ifndef VULKAN_LIB_TOOLS_HPP
#define VULKAN_LIB_TOOLS_HPP

#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <vector>
#include <functional>
#include <fstream>

#include "log/log.hpp"

// TODO: Move this to a better place
#ifdef PROJECT_SOURCE_DIR
[[maybe_unused]] static const std::string PSD = std::string(const_cast<char*>(PROJECT_SOURCE_DIR));
#endif

inline std::vector<char> ReadWholeFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        IO::PrintLog(LOG_LEVEL_WARNING, "Failed to read {}", filename);
        return std::vector<char>{};
    }

    auto size = file.tellg();
    std::vector<char> content(size);
    file.seekg(0);

    file.read(content.data(), content.size());

    IO::PrintLog(LOG_LEVEL_INFO, "Read {} successfully", filename);
    return content;
}

#endif // VULKAN_LIB_TOOLS_HPP
