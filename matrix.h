#ifndef CG6_MATRIX_H
#define CG6_MATRIX_H

#include <type_traits>
#include <cstring>
#include <SFML/Graphics.hpp>

template<int N>
class Matrix;

template<int N>
class MatrixPtr;

namespace sf {
    class Vector4f {
    public:
        float x;
        float y;
        float z;
        float w;

    public:
        explicit Vector4f() = default;
        explicit Vector4f(float x, float y, float z, float w);

        sf::Vector4f operator+(const sf::Vector4f& other);
        sf::Vector4f operator-(const sf::Vector4f& other);
        sf::Vector4f operator*(const Matrix<4>& m);
        sf::Vector4f operator*(const MatrixPtr<4>& m);

    };
}

template<int N>
struct Vector {};

template<>
struct Vector<4> { using type = sf::Vector4f; };

template<>
struct Vector<3> { using type = sf::Vector3f; };

template<>
struct Vector<2> { using type = sf::Vector2f; };

template<int N>
class Matrix {
    using VectorType = Vector<N>::type;
    friend class MatrixPtr<N>;

public:
    constexpr Matrix(const float values[N * N]) {
        for(int i = 0; i < N * N; ++i) {
            this->values[i] = values[i];
        }
    }

    Matrix(const std::vector<float>& values) {
        for(int i = 0; i < N * N; ++i) {
            this->values[i] = values[i];
        }
    }

    Matrix<N>& operator=(const Matrix<4>& other) {
        memcpy(values, other.values, sizeof(values));

        return *this;
    }

    static constexpr Matrix<N> zero() {
        float values[N * N];
        for(int i = 0; i < N * N; ++i) {
            values[i] = 0;
        }

        return Matrix(values);
    }

    static constexpr Matrix<N> identity() {
        float values[N * N];
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                values[i * N + j] = i == j ? 1 : 0;
            }
        }

        return Matrix(values);
    }

    Matrix<N> operator*(const Matrix<N>& other) const {
        Matrix<N> result = Matrix<N>::zero();
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                for(int k = 0; k < N; ++k) {
                    result(i, j) += operator()(i, k) * other(k, j);
                }
            }
        }

        return result;
    }

    Matrix<N> operator*(const MatrixPtr<N>& other) const {
        Matrix<N> result = Matrix<N>::zero();
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                for(int k = 0; k < N; ++k) {
                    result(i, j) += operator()(i, k) * other(k, j);
                }
            }
        }

        return result;
    }

    Matrix<N> operator+(Matrix<N>& other) {
        Matrix<N> result = Matrix<N>::zero();
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                result(i, j) = (*this)(i, j) + other(i, j);
            }
        }

        return result;
    }

    VectorType operator*(const VectorType& v) const {
        VectorType result;

        result.x = operator()(0, 0) * v.x + operator()(0, 1) * v.y;
        result.y = operator()(1, 0) * v.x + operator()(1, 1) * v.y;

        if constexpr(N >= 3) {
            result.x += operator()(0, 2) * v.z;
            result.y += operator()(1, 2) * v.z;
            result.z = operator()(2, 0) * v.x + operator()(2, 1) * v.y + operator()(2, 2) * v.z;
        }

        if constexpr(N >= 4) {
            result.x += operator()(0, 3) * v.w;
            result.y += operator()(1, 3) * v.w;
            result.z += operator()(2, 3) * v.w;
            result.w = operator()(3, 0) * v.x + operator()(3, 1) * v.y + operator()(3, 2) * v.z + operator()(3, 3) * v.w;
        }

        return result;
    }

    constexpr float operator()(int row, int col) const {
        return values[row * N + col];
    }

    constexpr float& operator()(int row, int col) {
        return values[row * N + col];
    }

private:
    float values[N * N];
};

template<int N>
class MatrixPtr {
    using VectorType = Vector<N>::type;

public:
    MatrixPtr() = default;

    constexpr MatrixPtr(float* values) {
        _values = values;
    }

    MatrixPtr<N>& operator=(const MatrixPtr<4>& other) {
        if(_values) {
            memcpy(_values, other._values, N * N * sizeof(float));
        } else {
            _values = other._values;
        }

        return *this;
    }

    void zero() {
        memset(_values, 0, N * N * sizeof(float));
    }

    void identity() {
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                _values[i * N + j] = i == j ? 1 : 0;
            }
        }
    }

    MatrixPtr<N>& operator=(const Matrix<4>& other) {
        memcpy(_values, other.values, N * N * sizeof(float));

        return *this;
    }

    MatrixPtr<N>& operator*=(const Matrix<N>& other) {
        Matrix<N> result = Matrix<N>::zero();
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                for(int k = 0; k < N; ++k) {
                    result(i, j) += operator()(i, k) * other(k, j);
                }
            }
        }

        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                (*this)(i, j) = result(i, j);
            }
        }

        return *this;
    }

    MatrixPtr<N>& operator*=(const MatrixPtr<N>& other) {
        Matrix<N> result = Matrix<N>::zero();
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                for(int k = 0; k < N; ++k) {
                    result(i, j) += operator()(i, k) * other(k, j);
                }
            }
        }

        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                (*this)(i, j) = result(i, j);
            }
        }

        return *this;
    }

    VectorType operator*(const VectorType& v) const {
        VectorType result;

        result.x = operator()(0, 0) * v.x + operator()(0, 1) * v.y;
        result.y = operator()(1, 0) * v.x + operator()(1, 1) * v.y;

        if constexpr(N >= 3) {
            result.x += operator()(0, 2) * v.z;
            result.y += operator()(1, 2) * v.z;
            result.z = operator()(2, 0) * v.x + operator()(2, 1) * v.y + operator()(2, 2) * v.z;
        }

        if constexpr(N >= 4) {
            result.x += operator()(0, 3) * v.w;
            result.y += operator()(1, 3) * v.w;
            result.z += operator()(2, 3) * v.w;
            result.w = operator()(3, 0) * v.x + operator()(3, 1) * v.y + operator()(3, 2) * v.z + operator()(3, 3) * v.w;
        }

        return result;
    }

    constexpr float operator()(int row, int col) const {
        return _values[row * N + col];
    }

    constexpr float& operator()(int row, int col) {
        return _values[row * N + col];
    }

private:
    float* _values = nullptr;
};

float dot(const sf::Vector3f& a, const sf::Vector3f& b);
sf::Vector3f cross(const sf::Vector3f& a, const sf::Vector3f& b);
sf::Vector3f normalize(const sf::Vector3f& vec);

#endif //CG6_MATRIX_H
