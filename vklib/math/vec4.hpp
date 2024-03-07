#ifndef VULKAN_LIB_VEC4_HPP
#define VULKAN_LIB_VEC4_HPP

#include "vulkan/vulkan.hpp"
#include <array>

class Vec4 final {
  public:
    Vec4() {
        data_.fill(0.0f);
    }

    Vec4(float x, float y, float z, float w) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    Vec4(std::initializer_list<float> list) {
        int i = 0;
        for (auto it = list.begin(); it != list.end(); it++) {
            data_[i++] = *it;
        }
    }

    float& operator[](int index) {
        return data_[index];
    }

    const float& operator[](int index) const {
        return data_[index];
    }

    Vec4& operator+=(const Vec4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] += other[i];
        }
        return *this;
    }

    Vec4& operator-=(const Vec4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] -= other[i];
        }
        return *this;
    }

    Vec4& operator*=(const Vec4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= other[i];
        }
        return *this;
    }

    Vec4& operator*=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= scalar;
        }
        return *this;
    }

    Vec4& operator/=(const Vec4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= other[i];
        }
        return *this;
    }

    Vec4& operator/=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= scalar;
        }
        return *this;
    }

    Vec4 operator+(const Vec4& other) const {
        Vec4 result = *this;
        result += other;
        return result;
    }

    Vec4 operator-(const Vec4& other) const {
        Vec4 result = *this;
        result -= other;
        return result;
    }

    Vec4 operator*(const Vec4& other) const {
        Vec4 result = *this;
        result *= other;
        return result;
    }

    Vec4 operator*(const float scalar) const {
        Vec4 result = *this;
        result *= scalar;
        return result;
    }

    Vec4 operator/(const Vec4& other) const {
        Vec4 result = *this;
        result /= other;
        return result;
    }

    Vec4 operator/(const float scalar) const {
        Vec4 result = *this;
        result /= scalar;
        return result;
    }

    Vec4& operator=(const Vec4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] = other[i];
        }
        return *this;
    }

    bool operator==(const Vec4& other) const {
        for (int i = 0; i < 4; i++) {
            if (data_[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    static float Dot(const Vec4& a, const Vec4& b) {
        float result = 0;
        for (int i = 0; i < 4; i++) {
            result += a[i] * b[i];
        }
        return result;
    }

    static Vec4 Cross(const Vec4& a, const Vec4& b) {
        return Vec4(a.y() * b.z() - a.z() * b.y(),
                    a.z() * b.x() - a.x() * b.z(),
                    a.x() * b.y() - a.y() * b.x(),
                    0.0f);
    }

    // Getter
    float x() const { return data_[0]; }
    float y() const { return data_[1]; }
    float z() const { return data_[2]; }
    float w() const { return data_[3]; }

    float GetLength() const {
        return std::sqrt(x() * x() + y() * y() + z() * z() + w() * w());
    }

    static vk::VertexInputAttributeDescription GetAttributeDescription() {
        vk::VertexInputAttributeDescription description;
        description
            .setBinding(0)
            .setLocation(0)
            .setFormat(vk::Format::eR32G32B32A32Sfloat)
            .setOffset(0);
        return description;
    }

    static vk::VertexInputBindingDescription GetBindingDescription() {
        vk::VertexInputBindingDescription description;
        description
            .setBinding(0)
            .setStride(sizeof(float) * 4)
            .setInputRate(vk::VertexInputRate::eVertex);
        return description;
    }

  private:
    std::array<float, 4> data_;
};

#endif // VULKAN_LIB_VEC4_HPP
