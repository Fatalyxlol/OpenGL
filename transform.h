#ifndef CG6_TRANSFORM_H
#define CG6_TRANSFORM_H

#include <SFML/Graphics.hpp>
#include "matrix.h"

enum class Axis {
    X,
    Y,
    Z
};

class Transform {
public:
    Transform(const sf::Vector3f &position, float* otwBuffer, float* wtoBuffer);
    virtual void moveBy(const sf::Vector3f &v);
    virtual void rotateAroundX(float angle);
    virtual void rotateAroundY(float angle);
    virtual void rotateAroundZ(float angle);
    virtual void rotateAroundLine(float angle, Axis axis);
    virtual void rotateAroundLine(const sf::Vector3f& dir, float angle);
    virtual void scaleAround(const sf::Vector3f &p, float kx, float ky, float kz);
    virtual void mirrorAroundAxis(Axis axis);
    virtual void transform(const Matrix<3> &m, const sf::Vector3f &v);
    virtual void transform(const Matrix<4> &m);

    MatrixPtr<4> objectToWorldMatrix() const;
    MatrixPtr<4> worldToObjectMatrix() const;
    sf::Vector3f toWorldCoordinates(const sf::Vector3f& vec) const;
    sf::Vector3f toObjectCoordinates(const sf::Vector3f& vec) const;
//    sf::Vector3f toWorldCoordinates(const sf::Vector3f& vec) const;
//    sf::Vector3f toObjectCoordinates(const sf::Vector3f& vec) const;
    sf::Vector3f getPosition() const { return position; }
    void setObjectToWorldMatrixPtr(float* ptr) { objectToWorld = MatrixPtr<4>(ptr); }
    void setWorldToObjectMatrixPtr(float* ptr) { worldToObject = MatrixPtr<4>(ptr); }
    void resetMatrices();

protected:
    Transform() = default;
    Matrix<4> rotationMatrix(const sf::Vector3f& dir, float angle);

protected:
    sf::Vector3f position;

protected:
    mutable MatrixPtr<4> objectToWorld;
    mutable MatrixPtr<4> worldToObject;
};



#endif //CG6_TRANSFORM_H
