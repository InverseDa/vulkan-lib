#ifndef VULKAN_LIB_VKARRAY_H
#define VULKAN_LIB_VKARRAY_H
#include "vulkan/vulkan.h"
#include <cstddef>

template <typename T>
class ArrayRef {
  public:
    // Constructor
    ArrayRef() = default;
    ArrayRef(T& data) : pArray(&data),
                        count(1) {}
    template <size_t elementCount>
    ArrayRef(T (&data)[elementCount]) : pArray(data),
                                        count(elementCount) {}
    ArrayRef(T* pData, size_t elementCount) : pArray(pData),
                                              count(elementCount) {}
    ArrayRef(const ArrayRef<std::remove_const_t<T>>& other) : pArray(other.Pointer()),
                                                              count(other.Count()) {}
    // Getter
    T* Pointer() const { return pArray; }
    size_t Count() const { return count; }
    // Operator
    T& operator[](size_t index) const { return pArray[index]; }
    ArrayRef& operator=(const ArrayRef&) = delete; // ban copy assignment and move assignment
    // Iterator
    T* begin() const { return pArray; }
    T* end() const { return pArray + count; }

  private:
    T* const pArray = nullptr;
    size_t count = 0;
};

#endif // VULKAN_LIB_VKARRAY_H
