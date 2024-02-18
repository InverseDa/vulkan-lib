#ifndef VULKAN_LIB_MACRO_H
#define VULKAN_LIB_MACRO_H

#include "core/vkBase.h"

#ifndef NDEBUG
#define ENABLE_DEBUG_MESSENGER true
#else
#define ENABLE_DEBUG_MESSENGER false
#endif

// Destroy Vulkan object handler
#define DestroyHandleBy(Func)                                           \
    if (handle) {                                                       \
        Func(GraphicsBase::GetInstance().GetDevice(), handle, nullptr); \
        handle = VK_NULL_HANDLE;                                        \
    }

// Move Vulkan object handler
#define MoveHandle         \
    handle = other.handle; \
    other.handle = VK_NULL_HANDLE;

// Copy Vulkan object handler
#define DefineHandleTypeOperator        \
    operator auto() const { \
        return handle;                  \
    }

// Get handle address
#define DefineAddressFunction                 \
    auto Address() const { \
        return &handle;                       \
    }

#define ExecuteOnce(...)              \
    {                                 \
        static bool executed = false; \
        if (executed) {               \
            return __VA_ARGS__;       \
        }                             \
        executed = true;              \
    }



#endif // VULKAN_LIB_MACRO_H
