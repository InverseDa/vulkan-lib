#ifndef VULKAN_LIB_MAT4_HPP
#define VULKAN_LIB_MAT4_HPP

#include "vulkan/vulkan.hpp"
#include <array>

#include "float4.hpp"
#include "float2.hpp"

class mat4 {
  public:
    mat4() {
        data_[0] = float4(1.0f, 0.0f, 0.0f, 0.0f);
        data_[1] = float4(0.0f, 1.0f, 0.0f, 0.0f);
        data_[2] = float4(0.0f, 0.0f, 1.0f, 0.0f);
        data_[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat4(const float4& v1, const float4& v2, const float4& v3, const float4& v4) {
        data_[0] = v1;
        data_[1] = v2;
        data_[2] = v3;
        data_[3] = v4;
    }

    mat4(float scale) {
        data_[0] = float4(scale, 0.0f, 0.0f, 0.0f);
        data_[1] = float4(0.0f, scale, 0.0f, 0.0f);
        data_[2] = float4(0.0f, 0.0f, scale, 0.0f);
        data_[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat4(const float4& scale) {
        data_[0] = float4(scale[0], 0.0f, 0.0f, 0.0f);
        data_[1] = float4(0.0f, scale[1], 0.0f, 0.0f);
        data_[2] = float4(0.0f, 0.0f, scale[2], 0.0f);
        data_[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat4(const mat4& other) {
        data_ = other.data_;
    }

    mat4& operator=(const mat4& other) {
        data_ = other.data_;
        return *this;
    }

    mat4& operator+=(const mat4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] += other.data_[i];
        }
        return *this;
    }

    mat4 operator+(const mat4& other) const {
        mat4 result = *this;
        result += other;
        return result;
    }

    mat4& operator-=(const mat4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] -= other.data_[i];
        }
        return *this;
    }

    mat4 operator-(const mat4& other) const {
        mat4 result = *this;
        result -= other;
        return result;
    }

    mat4& operator*=(const mat4& other) {
        mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result[j][i] = 0;
                for (int k = 0; k < 4; k++) {
                    result[j][i] += data_[k][i] * other.data_[j][k];
                }
            }
        }
        data_ = result.data_;
        return *this;
    }

    mat4 operator*(const mat4& other) const {
        mat4 result = *this;
        result *= other;
        return result;
    }

    float4 operator*(const float4& vec) const {
        float4 result;
        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result[i] += data_[j][i] * vec[j];
            }
        }
        return result;
    }

    float4& operator[](int index) {
        return data_[index];
    }

    mat4& Transpose() {
        for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 4; j++) {
                std::swap(data_[i][j], data_[j][i]);
            }
        }
        return *this;
    }

    static mat4 CreateIdentity() {
        return mat4();
    }

    static mat4 CreateOrtho(int left, int right, int top, int bottom, int near, int far) {
        mat4 mat;
        mat[0][0] = 2.0 / (right - left);
        mat[1][1] = 2.0 / (top - bottom);
        mat[2][2] = 2.0 / (near - far);
        mat[3][0] = (left + right) / (left - right);
        mat[3][1] = (top + bottom) / (bottom - top);
        mat[3][2] = (near + far) / (far - near);
        return mat;
    }

    static mat4 CreatePerspective(float fov, float aspect, float near, float far) {
        mat4 mat;
        float tanHalfFov = tan(fov / 2.f);
        mat[0][0] = 1.f / (aspect * tanHalfFov);
        mat[1][1] = 1.f / tanHalfFov;
        mat[2][2] = (far + near) / (near - far);
        mat[2][3] = -1;
        mat[3][2] = (2 * far * near) / (near - far);
        return mat;
    }

    static mat4 CreateTranslate(const float4& pos) {
        mat4 mat;
        mat[3] = pos;
        return mat;
    }

    static mat4 CreateTranslate(const float2& pos) {
        mat4 mat;
        mat[3][0] = pos.x();
        mat[3][1] = pos.y();
        return mat;
    }

    static mat4 CreateScale(const float4& scale) {
        mat4 mat;
        mat[0][0] = scale[0];
        mat[1][1] = scale[1];
        mat[2][2] = scale[2];
        return mat;
    }

    static mat4 CreateScale(const float2& scale) {
        mat4 mat;
        mat[0][0] = scale.x();
        mat[1][1] = scale.y();
        return mat;
    }

    static mat4 CreateScale(const float scale) {
        mat4 mat(scale);
        return mat;
    }

    float4* GetData() {
        return data_.data();
    }

  private:
    // Column-major order
    std::array<float4, 4> data_;
};

#endif // VULKAN_LIB_MAT4_HPP
