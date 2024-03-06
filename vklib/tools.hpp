#ifndef VULKAN_LIB_TOOLS_HPP
#define VULKAN_LIB_TOOLS_HPP

#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <vector>
#include <functional>
#include <fstream>

#include "log/log.hpp"

template <typename T, typename U>
void RemoveUnsupportedElems(std::vector<T>& elems, const std::vector<U>& supportedElems, std::function<bool(const T&, const U&)> eq) {
    int i = 0;
    while (i < elems.size()) {
        if (std::find_if(supportedElems.begin(), supportedElems.end(), [&](const U& elem) {
                return eq(elems[i], elem);
            }) == supportedElems.end()) {
            elems.erase(elems.begin() + i);
        } else {
            i++;
        }
    }
}

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
