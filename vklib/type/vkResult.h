#ifndef VULKAN_LIB_VKRESULT_H
#define VULKAN_LIB_VKRESULT_H

#include "vulkan/vulkan.h"

#define VK_RESULT_THROW

#ifdef VK_RESULT_THROW
class ResultType {
  public:
    static void (*callbackThrow)(VkResult);
    ResultType(VkResult result) : result(result){};
    ResultType(ResultType&& other) noexcept : result(other.result) { other.result = VK_SUCCESS; }
    ~ResultType() noexcept(false) {
        if (uint32_t(result) < VK_RESULT_MAX_ENUM)
            return;
        if (callbackThrow)
            callbackThrow(result);
        throw result;
    }

    operator VkResult() {
        VkResult result = this->result;
        this->result = VK_SUCCESS;
        return result;
    };

    ResultType& operator=(ResultType&& other) noexcept {
        result = other.result;
        other.result = VK_SUCCESS;
        return *this;
    }

  private:
    VkResult result;
};
inline void (*ResultType::callbackThrow)(VkResult);

#elif defined VK_RESULT_NODISCARD
struct [[nodiscard]] ResultType {
    VkResult result;
    ResultType(VkResult result) : result(result) {}
    operator VkResult() const { return result; }
};
#pragma warning(disable : 4834)
#pragma warning(disable : 6031)

#else
using ResultType = VkResult;

#endif

#endif // VULKAN_LIB_VKRESULT_H
