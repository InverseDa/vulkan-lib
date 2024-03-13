#ifndef VULKAN_LIB_FLOAT3_HPP
#define VULKAN_LIB_FLOAT3_HPP

class float3 final {
  public:
    float3() {
        data_.fill(0.0f);
    }

    float3(float x, float y, float z) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
    }

    float3(std::initializer_list<float> list) {
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

    float3& operator+=(const float3& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] += other[i];
        }
        return *this;
    }

    float3& operator-=(const float3& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] -= other[i];
        }
        return *this;
    }

    float3& operator*=(const float3& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= other[i];
        }
        return *this;
    }

    float3& operator*=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] *= scalar;
        }
        return *this;
    }

    float3& operator/=(const float3& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= other[i];
        }
        return *this;
    }

    float3& operator/=(const float scalar) {
        for (int i = 0; i < 4; i++) {
            data_[i] /= scalar;
        }
        return *this;
    }

    float3 operator+(const float3& other) const {
        float3 result = *this;
        result += other;
        return result;
    }

    float3 operator-(const float3& other) const {
        float3 result = *this;
        result -= other;
        return result;
    }

    float3 operator*(const float3& other) const {
        float3 result = *this;
        result *= other;
        return result;
    }

    float3 operator*(const float scalar) const {
        float3 result = *this;
        result *= scalar;
        return result;
    }

    float3 operator/(const float3& other) const {
        float3 result = *this;
        result /= other;
        return result;
    }

    float3 operator/(const float scalar) const {
        float3 result = *this;
        result /= scalar;
        return result;
    }

    float3& operator=(const float3& other) {
        for (int i = 0; i < 4; i++) {
            data_[i] = other[i];
        }
        return *this;
    }

    bool operator==(const float3& other) const {
        for (int i = 0; i < 4; i++) {
            if (data_[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    static float Dot(const float3& a, const float3& b) {
        float result = 0;
        for (int i = 0; i < 3; i++) {
            result += a[i] * b[i];
        }
        return result;
    }

    static float3 Cross(const float3& a, const float3& b) {
        return float3(a.y() * b.z() - a.z() * b.y(),
                    a.z() * b.x() - a.x() * b.z(),
                    a.x() * b.y() - a.y() * b.x());
    }

    // Getter
    float x() const { return data_[0]; }
    float y() const { return data_[1]; }
    float z() const { return data_[2]; }

    float GetLength() const {
        return std::sqrt(x() * x() + y() * y() + z() * z());
    }

  private:
    std::array<float, 3> data_;
};

#endif // VULKAN_LIB_FLOAT3_HPP
