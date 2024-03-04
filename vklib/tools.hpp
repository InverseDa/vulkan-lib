#ifndef VULKAN_LIB_TOOLS_HPP
#define VULKAN_LIB_TOOLS_HPP

#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <vector>
#include <functional>

using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

namespace Vklib {
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
} // namespace Vklib

#endif // VULKAN_LIB_TOOLS_HPP
