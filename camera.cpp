#include <cmath>
#include "camera.h"

Camera::Camera(const sf::Vector3f &position, float *otw_buffer, float *wto_buffer)
    : Transform(position, otw_buffer,wto_buffer)
{ }

void Camera::rotateAroundX(float angle) {
    _rotation.x += angle;
    rotate();
}

void Camera::rotateAroundY(float angle) {
    _rotation.y += angle;
    rotate();
}

void Camera::rotate() {
    float angle = _rotation.x * M_PI / 180;
    float sina = sin(angle);
    float cosa = cos(angle);

    Matrix<4> otw = Matrix<4>::identity();
    otw(1, 1) = cosa;
    otw(1, 2) = sina;
    otw(2, 1) = -sina;
    otw(2, 2) = cosa;

    Matrix<4> wto = otw;
    wto(1, 2) = -sina;
    wto(2, 1) = sina;

    angle = _rotation.y * M_PI / 180;
    sina = sin(angle);
    cosa = cos(angle);
    Matrix<4> m = Matrix<4>::identity();
    m(0, 0) = cosa;
    m(0, 2) = sina;
    m(2, 0) = -sina;
    m(2, 2) = cosa;

    otw = m * otw;

    m(0, 2) = -sina;
    m(2, 0) = sina;

    wto = wto * m;

    angle = _rotation.z * M_PI / 180;
    sina = sin(angle);
    cosa = cos(angle);
    m = Matrix<4>::identity();
    m(0, 0) = cosa;
    m(0, 1) = sina;
    m(1, 0) = -sina;
    m(1, 1) = cosa;

    objectToWorld = m * otw;

    m(0, 1) = -sina;
    m(1, 0) = sina;

    worldToObject = wto * m;
}

void Camera::rotateAroundLine(const sf::Vector3f &dir, float angle) {
    _rotation.x += dir.x * dir.x * angle;
    _rotation.y += dir.y * dir.y * angle;
    _rotation.z += dir.z * dir.z * angle;

    auto tmp = position;
    moveBy(-tmp);
    rotate();
    moveBy(tmp);
}

void Camera::rotateAroundLine(float angle, Axis axis) {
    Transform::rotateAroundLine(angle, axis);
}
