#include <cmath>
#include "matrix.h"

sf::Vector4f::Vector4f(float x, float y, float z, float w): x(x), y(y), z(z), w(w)
{ }

sf::Vector4f sf::Vector4f::operator+(const sf::Vector4f &other) {
    return sf::Vector4f(x + other.x, y + other.y, z + other.z, w + other.w);
}

sf::Vector4f sf::Vector4f::operator-(const sf::Vector4f &other) {
    return sf::Vector4f(x + other.x, y + other.y, z + other.z, w + other.w);
}

sf::Vector4f sf::Vector4f::operator*(const Matrix<4> &m) {
    sf::Vector4f result(0, 0, 0, 0);
    result.x += x * m(0, 0);
    result.x += y * m(1, 0);
    result.x += z * m(2, 0);
    result.x += w * m(3, 0);

    result.y += x * m(0, 1);
    result.y += y * m(1, 1);
    result.y += z * m(2, 1);
    result.y += w * m(3, 1);

    result.z += x * m(0, 2);
    result.z += y * m(1, 2);
    result.z += z * m(2, 2);
    result.z += w * m(3, 2);

    result.w += x * m(0, 3);
    result.w += y * m(1, 3);
    result.w += z * m(2, 3);
    result.w += w * m(3, 3);

    return result;
}

sf::Vector4f sf::Vector4f::operator*(const MatrixPtr<4> &m) {
    sf::Vector4f result(0, 0, 0, 0);
    result.x += x * m(0, 0);
    result.x += y * m(1, 0);
    result.x += z * m(2, 0);
    result.x += w * m(3, 0);

    result.y += x * m(0, 1);
    result.y += y * m(1, 1);
    result.y += z * m(2, 1);
    result.y += w * m(3, 1);

    result.z += x * m(0, 2);
    result.z += y * m(1, 2);
    result.z += z * m(2, 2);
    result.z += w * m(3, 2);

    result.w += x * m(0, 3);
    result.w += y * m(1, 3);
    result.w += z * m(2, 3);
    result.w += w * m(3, 3);

    return result;
}

float dot(const sf::Vector3f& a, const sf::Vector3f& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

sf::Vector3f normalize(const sf::Vector3f& vec) {
    auto magnitude = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return sf::Vector3f(vec.x / magnitude, vec.y / magnitude, vec.z / magnitude);
}

sf::Vector3f cross(const sf::Vector3f& first, const sf::Vector3f& second) {
    return {first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x};
}