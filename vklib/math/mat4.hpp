#ifndef VULKAN_LIB_MAT4_HPP
#define VULKAN_LIB_MAT4_HPP

#include "vulkan/vulkan.hpp"
#include <array>

#include "vec4.hpp"
#include "vec2.hpp"

class Mat4 {
  public:
    Mat4() {
        data_[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
        data_[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
        data_[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
        data_[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Mat4(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Vec4& v4) {
        data_[0] = v1;
        data_[1] = v2;
        data_[2] = v3;
        data_[3] = v4;
    }

    Mat4(float scale) {
        data_[0] = Vec4(scale, 0.0f, 0.0f, 0.0f);
        data_[1] = Vec4(0.0f, scale, 0.0f, 0.0f);
        data_[2] = Vec4(0.0f, 0.0f, scale, 0.0f);
        data_[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Mat4(const Vec4& scale) {
        data_[0] = Vec4(scale[0], 0.0f, 0.0f, 0.0f);
        data_[1] = Vec4(0.0f, scale[1], 0.0f, 0.0f);
        data_[2] = Vec4(0.0f, 0.0f, scale[2], 0.0f);
        data_[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Mat4(const Mat4& other) {
        data_ = other.data_;
    }

    Mat4& operator=(const Mat4& other) {
        data_ = other.data_;
        return *this;
    }

    Mat4& operator+=(const Mat4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] += other.data_[i];
        }
        return *this;
    }

    Mat4 operator+(const Mat4& other) const {
        Mat4 result = *this;
        result += other;
        return result;
    }

    Mat4& operator-=(const Mat4& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] -= other.data_[i];
        }
        return *this;
    }

    Mat4 operator-(const Mat4& other) const {
        Mat4 result = *this;
        result -= other;
        return result;
    }

    Mat4& operator*=(const Mat4& other) {
        Mat4 result;
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

    Mat4 operator*(const Mat4& other) const {
        Mat4 result = *this;
        result *= other;
        return result;
    }

    Vec4 operator*(const Vec4& vec) const {
        Vec4 result;
        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result[i] += data_[j][i] * vec[j];
            }
        }
        return result;
    }

    Vec4& operator[](int index) {
        return data_[index];
    }

    Mat4& Transpose() {
        for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 4; j++) {
                std::swap(data_[i][j], data_[j][i]);
            }
        }
        return *this;
    }

    static Mat4 CreateIdentity() {
        return Mat4();
    }

    static Mat4 CreateOrtho(int left, int right, int top, int bottom, int near, int far) {
        Mat4 mat;
        mat[0][0] = 2.0 / (right - left);
        mat[1][1] = 2.0 / (top - bottom);
        mat[2][2] = 2.0 / (near - far);
        mat[3][0] = (left + right) / (left - right);
        mat[3][1] = (top + bottom) / (bottom - top);
        mat[3][2] = (near + far) / (far - near);
        return mat;
    }

    static Mat4 CreatePerspective(float fov, float aspect, float near, float far) {
        Mat4 mat;
        float tanHalfFov = tan(fov / 2.f);
        mat[0][0] = 1.f / (aspect * tanHalfFov);
        mat[1][1] = 1.f / tanHalfFov;
        mat[2][2] = (far + near) / (near - far);
        mat[2][3] = -1;
        mat[3][2] = (2 * far * near) / (near - far);
        return mat;
    }

    static Mat4 CreateTranslate(const Vec4& pos) {
        Mat4 mat;
        mat[3] = pos;
        return mat;
    }

    static Mat4 CreateTranslate(const Vec2& pos) {
        Mat4 mat;
        mat[3][0] = pos.x;
        mat[3][1] = pos.y;
        return mat;
    }

    static Mat4 CreateScale(const Vec4& scale) {
        Mat4 mat;
        mat[0][0] = scale[0];
        mat[1][1] = scale[1];
        mat[2][2] = scale[2];
        return mat;
    }

    static Mat4 CreateScale(const Vec2& scale) {
        Mat4 mat;
        mat[0][0] = scale.x;
        mat[1][1] = scale.y;
        return mat;
    }

    static Mat4 CreateScale(const float scale) {
        Mat4 mat(scale);
        return mat;
    }

    Vec4* GetData() {
        return data_.data();
    }

  private:
    // Column-major order
    std::array<Vec4, 4> data_;
};

#endif // VULKAN_LIB_MAT4_HPP
