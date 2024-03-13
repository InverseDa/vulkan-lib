#ifndef VULKAN_LIB_FLOAT4_HPP
#define VULKAN_LIB_FLOAT4_HPP

#include "vulkan/vulkan.hpp"
#include <array>

class float4 final {
  public:
    float4() {
        data_.fill(0.0f);
    }

    float4(float x, float y, float z, float w) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
    }

    float4(std::initializer_list<float> list) {
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

    float4& operator+=(const float4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] += other[i];
        }
        return *this;
    }

    float4& operator-=(const float4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] -= other[i];
        }
        return *this;
    }

    float4& operator*=(const float4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= other[i];
        }
        return *this;
    }

    float4& operator*=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= scalar;
        }
        return *this;
    }

    float4& operator/=(const float4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= other[i];
        }
        return *this;
    }

    float4& operator/=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= scalar;
        }
        return *this;
    }

    float4 operator+(const float4& other) const {
        float4 result = *this;
        result += other;
        return result;
    }

    float4 operator-(const float4& other) const {
        float4 result = *this;
        result -= other;
        return result;
    }

    float4 operator*(const float4& other) const {
        float4 result = *this;
        result *= other;
        return result;
    }

    float4 operator*(const float scalar) const {
        float4 result = *this;
        result *= scalar;
        return result;
    }

    float4 operator/(const float4& other) const {
        float4 result = *this;
        result /= other;
        return result;
    }

    float4 operator/(const float scalar) const {
        float4 result = *this;
        result /= scalar;
        return result;
    }

    float4& operator=(const float4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] = other[i];
        }
        return *this;
    }

    bool operator==(const float4& other) const {
        for (int i = 0; i < 4; i++) {
            if (data_[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    static float Dot(const float4& a, const float4& b) {
        float result = 0;
        for (int i = 0; i < 4; i++) {
            result += a[i] * b[i];
        }
        return result;
    }

    static float4 Cross(const float4& a, const float4& b) {
        return float4(a.y() * b.z() - a.z() * b.y(),
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

  private:
    std::array<float, 4> data_;
};

#endif // VULKAN_LIB_FLOAT4_HPP
