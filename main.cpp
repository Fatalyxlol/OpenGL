#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <chrono>
#include "object.h"
#include "camera.h"


GLuint Program;
GLint Attrib_vertex;
GLint Attrib_texture;
GLint Attrib_transformation;
GLint UniformCameraTransform;
GLint unif_sampler1;
float camWto[16];


const char *VertexShaderSource2 = R"(
    #version 330 core
    in vec3 coord;
    in vec2 vertexTextureCoords;
    in mat4 transformation;

    uniform mat4 cameraTransform;

    out vec2 vTextureCoordinate;

    void main() {
        vec3 position = vec3(coord);
        float far = 100.0;
        float near = 0.01;
        float fov = 1.22173;
        mat4 projection = mat4(
            1 / tan(fov / 2), 0, 0, 0,
            0, 1 / tan(fov / 2), 0, 0,
            0, 0, (far + near) / (far - near), 1,
            0, 0, -(2 * far * near) / (far - near), 0
        );

        vec4 pos = projection * (cameraTransform * (transformation * vec4(position.xyz, 1.0)));
        //pos = projection * pos;
        gl_Position = pos;
        vTextureCoordinate = vertexTextureCoords;
    }
)";

const char *FragShaderSource2 = R"(
    #version 330 core

    in vec2 vTextureCoordinate;

    uniform sampler2D ourTexture1;

    out vec4 color;
    void main() {
        color = texture(ourTexture1, vTextureCoordinate);
    }
)";

const char *vertex_shader = VertexShaderSource2;
const char *fragment_shader = FragShaderSource2;

void InitVBO() {
    for (auto [_, obj]: ObjectRepository::instance().objects()) {
        obj->createVAO();
        glBindVertexArray(obj->VAO());

        glEnableVertexAttribArray(Attrib_vertex);
        glEnableVertexAttribArray(Attrib_texture);

        glBindBuffer(GL_ARRAY_BUFFER, obj->VBO());
        glBufferData(GL_ARRAY_BUFFER, obj->vertices().size() * sizeof(VertexData), obj->vertices().data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
        glVertexAttribPointer(Attrib_texture, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                              (void *) (sizeof(GLfloat) * 3));

        glEnableVertexAttribArray(Attrib_transformation);
        glEnableVertexAttribArray(Attrib_transformation + 1);
        glEnableVertexAttribArray(Attrib_transformation + 2);
        glEnableVertexAttribArray(Attrib_transformation + 3);

        glBindBuffer(GL_ARRAY_BUFFER, obj->matrices());
        auto info = ObjectRepository::instance().instances().at(obj);
        glBufferData(GL_ARRAY_BUFFER, info.count * 16 * sizeof(float), info.otwMatrices, GL_STREAM_DRAW);
        glVertexAttribPointer(Attrib_transformation, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 0);
        glVertexAttribPointer(Attrib_transformation + 1, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                              (void *) (4 * sizeof(float)));
        glVertexAttribPointer(Attrib_transformation + 2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                              (void *) (8 * sizeof(float)));
        glVertexAttribPointer(Attrib_transformation + 3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                              (void *) (12 * sizeof(float)));

        glVertexAttribDivisor(Attrib_transformation, 1);
        glVertexAttribDivisor(Attrib_transformation + 1, 1);
        glVertexAttribDivisor(Attrib_transformation + 2, 1);
        glVertexAttribDivisor(Attrib_transformation + 3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
}

void InitShader() {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertex_shader, NULL);
    glCompileShader(vShader);
    std::cout << "vertex shader \n";

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragment_shader, NULL);
    glCompileShader(fShader);
    std::cout << "fragment shader \n";

    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);

    glLinkProgram(Program);
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        std::cout << "error attach shaders \n";
        return;
    }

    Attrib_vertex = glGetAttribLocation(Program, "coord");
    if (Attrib_vertex == -1) {
        std::cout << "could not bind attrib coord" << std::endl;
        return;
    }

    Attrib_transformation = glGetAttribLocation(Program, "transformation");
    if (Attrib_transformation == -1) {
        std::cout << "could not bind attrib transform" << std::endl;
        return;
    }

    Attrib_texture = glGetAttribLocation(Program, "vertexTextureCoords");
    if (Attrib_texture == -1) {
        std::cout << "could not bind attrib texture" << std::endl;
        return;
    }

    UniformCameraTransform = glGetUniformLocation(Program, "cameraTransform");
    if (UniformCameraTransform == -1) {
        std::cout << "could not bind camera transform" << std::endl;
        return;
    }

    unif_sampler1 = glGetUniformLocation(Program, "ourTexture1");
    if (unif_sampler1 == -1) {
        std::cout << "could not bind uniform " << "ourTexture1" << std::endl;
        return;
    }
}

void Init() {
    InitShader();
    InitVBO();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}


void Draw() {
    glUseProgram(Program);

    glUniform1i(unif_sampler1, 0);
    glUniformMatrix4fv(UniformCameraTransform, 1, GL_FALSE, camWto);

    for (auto [name, obj]: ObjectRepository::instance().objects()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, obj->texture().getNativeHandle());

        glBindBuffer(GL_ARRAY_BUFFER, obj->matrices());
        auto count = ObjectRepository::instance().instances().at(obj).count;
        glBufferData(GL_ARRAY_BUFFER, 16 * count * sizeof(float), obj->otwMatrices(), GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(obj->VAO());
        glDrawArraysInstanced(GL_TRIANGLES, 0, obj->vertices().size(), count);
        glBindVertexArray(0);
    }


    glUseProgram(0);
}


int main() {
    sf::Window window(sf::VideoMode(1000, 1000), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);
    glewInit();

    vertex_shader = VertexShaderSource2;
    fragment_shader = FragShaderSource2;

    /*auto obj = Object::load("pig.obj", "texture3.jpg");
    auto count = 5;
    auto instances = ObjectRepository::instance().alloc(obj.name(), count);
    for(int i = 0; i < count; ++i) {
        auto& model = instances[i];
        //model.rotateAroundX(90);
        model.moveBy({(float)(i + 1) * 3, 0, 0});
        model.rotateAroundY(360 / count *//** i * (1 - 2 * (i % 2))*//*);
    }*/
    auto pig = Object::load("pig.obj", "Tiger_barb_diff.jpg");
    auto pigInstances = ObjectRepository::instance().alloc(pig.name(), 5);
    pigInstances[0].rotateAroundY(180);
    pigInstances[0].moveBy({-0.7,-0.6,0.4});
    pigInstances[1].rotateAroundY(180);
    pigInstances[1].moveBy({0,-0.6,0.4});
    pigInstances[2].rotateAroundY(180);
    pigInstances[2].moveBy({0.7,-0.6,0.4});
    pigInstances[3].rotateAroundY(180);
    pigInstances[3].moveBy({-0.35,0,0.4});
    pigInstances[4].rotateAroundY(180);
    pigInstances[4].moveBy({0.35,0,0.4});

    auto cat  = Object::load("cat.obj","cat_diff.tga");
    auto catInstance = ObjectRepository::instance().alloc(cat.name(),1);
    catInstance[0].rotateAroundY(180);
    catInstance[0].moveBy({0,0.6,0.4});

    float camOtw[16];
    auto camera = Camera({0, 0, -2}, camOtw, camWto);

    auto room = Object::load("room.obj", "texture3.jpg");
    auto walls = 5;
    auto roomInstances = ObjectRepository::instance().alloc(room.name(), walls);
    for (int i = 0; i < walls; ++i) {
        auto &model = roomInstances[i];
        model.moveBy({0, 0, 1});
        //model.rotateAroundY(360 / count /** i * (1 - 2 * (i % 2))*/);
    }
    roomInstances[2].rotateAroundY(90);
    roomInstances[3].rotateAroundY(270);
    roomInstances[4].rotateAroundX(90);
    roomInstances[1].rotateAroundX(270);
    Init();

    bool mousePressed = false;
    sf::Vector2i mousePrev;
    float speed = 0.1f;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case (sf::Keyboard::W): {
                        auto v = normalize(camera.toWorldCoordinates({0, 0, 1}) - camera.getPosition());
                        v *= speed;
                        camera.moveBy(v);
                        break;
                    }
                    case (sf::Keyboard::S): {
                        auto v = normalize(camera.toWorldCoordinates({0, 0, -1}) - camera.getPosition());
                        v *= speed;
                        camera.moveBy(v);
                        break;
                    }
                    case (sf::Keyboard::A): {
                        auto v = normalize(camera.toWorldCoordinates({-1, 0, 0}) - camera.getPosition());
                        v *= speed;
                        camera.moveBy(v);
                        break;
                    }
                    case (sf::Keyboard::D): {
                        auto v = normalize(camera.toWorldCoordinates({1, 0, 0}) - camera.getPosition());
                        v *= speed;
                        camera.moveBy(v);
                        break;
                    }
                    case (sf::Keyboard::Q): {
                        auto v = normalize(camera.toWorldCoordinates({0, 1, 0}) - camera.getPosition());
                        v *= speed;
                        camera.moveBy(v);
                        break;
                    }
                    case (sf::Keyboard::E): {
                        auto v = normalize(camera.toWorldCoordinates({0, -1, 0}) - camera.getPosition());
                        v *= speed;
                        camera.moveBy(v);
                        break;
                    }
                    default:
                        break;
                }
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Button::Left) {
                mousePressed = true;
            } else if (event.type == sf::Event::MouseButtonReleased &&
                       event.mouseButton.button == sf::Mouse::Button::Left) {
                mousePressed = false;
            }
        }

        if (mousePressed) {
            auto diff = mousePrev - sf::Mouse::getPosition();
            if (diff.x != 0) {
                auto angle = (float) diff.x / window.getSize().x * 80;
//                auto v = normalize(camera.toWorldCoordinates({0, 0, 1}) - camera.getPosition());
//                auto x = normalize(cross(v, {1, 0, 0}));
//                camera.rotateAroundLine(x, angle);
//                camera.rotateAroundLine(normalize(camera.toObjectCoordinates(sf::Vector3f {0, 1, 0} + camera.getPosition())), -angle);
//                camera.rotateAroundLine({0, 1, 0}, -angle);
                camera.rotateAroundLine(angle, Axis::Y);
//                camera.rotateAroundY(angle);
            }

            if (diff.y != 0) {
                auto angle = (float) diff.y / window.getSize().y * 80;
//                auto v = normalize(camera.toWorldCoordinates({1, 0, 0}) - camera.getPosition());
//                auto x = normalize(cross(v, normalize(camera.toWorldCoordinates({0, 1, 0}) - camera.getPosition())));
//                camera.rotateAroundLine(x, -angle);
//                camera.rotateAroundLine(normalize(camera.toObjectCoordinates(sf::Vector3f {1, 0, 0} + camera.getPosition())), -angle);
                camera.rotateAroundLine(-angle, Axis::X);
//                camera.rotateAroundX(angle);
            }
        }

        mousePrev = sf::Mouse::getPosition();

        /*for(int i = 0; i < instances.size(); ++i) {
            auto& model = instances[i];
            model.rotateAroundY(2.0f / pow(i + 1, 1.3));
            model.rotateAroundLine(3.0f / pow(i + 1, 1.2), Axis::Y);
        }

        catInstance.rotateAroundLine(1, Axis::Y);*/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();

        window.display();
    }

    return 0;
}