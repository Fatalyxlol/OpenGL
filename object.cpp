#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/texture.h>
#include <iostream>
#include "matrix.h"
#include "object.h"

Object Object::load(const std::string &path, const std::string& texturePath) {
    Object obj;

    obj.loadModel(path);
    if(!texturePath.empty()) {
        obj.loadTexture(texturePath);
    }

    return obj;
}

void Object::loadModel(const std::string &path) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_FlipWindingOrder);
    std::vector<VertexData> vertices;
    float max = 0;

    for(int i = 0; i < scene->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[i];

        for(int j = 0; j < mesh->mNumFaces; ++j) {
            auto face = mesh->mFaces[j];

            for(int k = 0; k < face.mNumIndices; ++k) {
                auto idx = face.mIndices[k];
                auto vertex = mesh->mVertices[idx];
                auto tex = mesh->mTextureCoords[0][idx];
                if(std::abs(vertex.x) > max) {
                    max = std::abs(vertex.x);
                }
                if(std::abs(vertex.y) > max) {
                    max = std::abs(vertex.y);
                }
                if(std::abs(vertex.z) > max) {
                    max = std::abs(vertex.z);
                }

                sf::Vector3f v {vertex.x, vertex.y, vertex.z};

                vertices.push_back({v, {tex.x, tex.y}});
            }
        }
    }

    Matrix<4> m {{
                         1 / max, 0,       0,       0,
                         0,       1 / max, 0,       0,
                         0,       0,       1 / max, 0,
                         0,       0,       0,       1
                 }};

    sf::Vector3f center {0, 0, 0};
    for(auto& vertex: vertices) {
        sf::Vector4f tmp { vertex.coords.x, vertex.coords.y, vertex.coords.z, 1 };
        tmp = tmp * m;
        vertex.coords.x = tmp.x;
        vertex.coords.y = tmp.y;
        vertex.coords.z = tmp.z;

        center += vertex.coords;
    }

    center = {center.x / vertices.size(), center.y / vertices.size(), center.z / vertices.size()};
    for(auto& vertex: vertices) {
        vertex.coords -= center;
    }

    _vertices = std::move(vertices);
    _name = path;
    ObjectRepository::instance().objectLoaded(_name, this);
}

void Object::loadTexture(const std::string &path) {
    if(!_texture.loadFromFile(path)) {
        std::cout << "Could not load texture " << path << std::endl;
        return;
    }
}

Transform Object::getInstance(float *otwBuffer, float *wtoBuffer) const {
    return {{0, 0, 0}, otwBuffer, wtoBuffer};
}

void Object::createVAO() const {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_matrices);
}

float *Object::otwMatrices() const {
    return ObjectRepository::instance().instances().at(this).otwMatrices;
}

std::vector<Transform> ObjectRepository::alloc(const std::string &name, int count) {
    std::vector<Transform> result;
    result.reserve(count);
    auto* otwMatrices = new float[16 * count];
    auto* wtoMatrices = new float[16 * count];
    auto obj = _objects[name];
    _instances[obj].count = count;
    _instances[obj].otwMatrices = otwMatrices;
    _instances[obj].wtoMatrices = wtoMatrices;

    for (int i = 0; i < count; ++i) {
        result.push_back(obj->getInstance(otwMatrices + i * 16, wtoMatrices + i * 16));
    }

    return result;
}

void ObjectRepository::objectLoaded(const std::string &name, const Object *obj) {
    _objects[name] = obj;
    _instances[obj] = {0, nullptr, nullptr};
}

ObjectRepository &ObjectRepository::instance() {
    static ObjectRepository repository;
    return repository;
}
