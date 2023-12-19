#ifndef LAB1GL_CAMERA_H
#define LAB1GL_CAMERA_H

#include "transform.h"

class Camera: public Transform {
public:
    Camera(const sf::Vector3f &position, float *otw_buffer, float *wto_buffer);
    void rotateAroundX(float angle) override;
    void rotateAroundY(float angle) override;
    void rotateAroundLine(const sf::Vector3f &dir, float angle) override;
    void rotateAroundLine(float angle, Axis axis) override;

private:
    void rotate();

private:
    sf::Vector3f _rotation;
};


#endif //LAB1GL_CAMERA_H
