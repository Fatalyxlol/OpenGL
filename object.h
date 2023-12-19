#ifndef LAB1GL_OBJECT_H
#define LAB1GL_OBJECT_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "transform.h"

using namespace sf::Glsl;

struct VertexData {
    Vec3 coords;
    Vec2 texCoords;
};

struct InstancesInfo {
    int count;
    float* otwMatrices;
    float* wtoMatrices;
};

class ObjectRepository;

class Object {
public:
    static Object load(const std::string& path, const std::string& texturePath = {});
    Transform getInstance(float* otwBuffer, float* wtoBuffer) const;
    void createVAO() const;

    const sf::Texture& texture() const { return _texture; }
    const std::vector<VertexData>& vertices() const { return _vertices; }
    const std::string& name() const { return _name; }
    GLuint VAO() const { return _vao; }
    GLuint VBO() const { return _vbo; }
    GLuint matrices() const { return _matrices; }
    float* otwMatrices() const;

private:
    Object() = default;
    void loadTexture(const std::string& path);
    void loadModel(const std::string& path);

private:
    std::vector<VertexData> _vertices;
    sf::Texture _texture;
    std::string _name;
    mutable GLuint _vao;
    mutable GLuint _vbo;
    mutable GLuint _matrices;
};

class ObjectRepository {
    friend class Object;

public:
    std::vector<Transform> alloc(const std::string& name, int count);
    float* getMatrices(const std::string& name) { return _instances[_objects[name]].otwMatrices; }
    const std::map<std::string, const Object*>& objects() const { return _objects; }
    const std::map<const Object*, InstancesInfo>& instances() const { return _instances; }

    static ObjectRepository& instance();

private:
    void objectLoaded(const std::string& name, const Object* obj);

private:
    std::map<const Object*, InstancesInfo> _instances;
    std::map<std::string, const Object*> _objects;
};


#endif //LAB1GL_OBJECT_H
