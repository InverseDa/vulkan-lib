#ifndef VULKAN_LIB_FLOAT2_HPP
#define VULKAN_LIB_FLOAT2_HPP

#include "vulkan/vulkan.hpp"

class float2 final {
  public:
    float2() {
        data_.fill(0.0f);
    }

    float2(float x, float y, float z) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
    }

    float2(std::initializer_list<float> list) {
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

    float2& operator+=(const float2& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] += other[i];
        }
        return *this;
    }

    float2& operator-=(const float2& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] -= other[i];
        }
        return *this;
    }

    float2& operator*=(const float2& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= other[i];
        }
        return *this;
    }

    float2& operator*=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= scalar;
        }
        return *this;
    }

    float2& operator/=(const float2& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= other[i];
        }
        return *this;
    }

    float2& operator/=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= scalar;
        }
        return *this;
    }

    float2 operator+(const float2& other) const {
        float2 result = *this;
        result += other;
        return result;
    }

    float2 operator-(const float2& other) const {
        float2 result = *this;
        result -= other;
        return result;
    }

    float2 operator*(const float2& other) const {
        float2 result = *this;
        result *= other;
        return result;
    }

    float2 operator*(const float scalar) const {
        float2 result = *this;
        result *= scalar;
        return result;
    }

    float2 operator/(const float2& other) const {
        float2 result = *this;
        result /= other;
        return result;
    }

    float2 operator/(const float scalar) const {
        float2 result = *this;
        result /= scalar;
        return result;
    }

    float2& operator=(const float2& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] = other[i];
        }
        return *this;
    }

    bool operator==(const float2& other) const {
        for (int i = 0; i < 4; i++) {
            if (data_[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    static float Dot(const float2& a, const float2& b) {
        float result = 0;
        for (int i = 0; i < 2; i++) {
            result += a[i] * b[i];
        }
        return result;
    }

    static float Cross(const float2& a, const float2& b) {
        return a[0] * b[1] - a[1] * b[0];
    }

    // Getter
    float x() const { return data_[0]; }
    float y() const { return data_[1]; }

    float GetLength() const {
        return std::sqrt(x() * x() + y() * y());
    }

  private:
    std::array<float, 2> data_;
};

#endif // VULKAN_LIB_FLOAT2_HPP
