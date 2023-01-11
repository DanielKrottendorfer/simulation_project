#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "opengl_util.hpp"
#include "settings.hpp"

#include "vertex_data.hpp"

struct GameObject
{

    gl_util::Mesh m_mesh;
    gl_util::Texture m_texture;

    glm::vec2 m_position;
    glm::vec2 m_heading;
    glm::vec2 m_force;

    float m_radius;

    float m_mass;

    static GameObject new_2d_sprite(const char *tex_path);
    void calc_gravity(const GameObject &other);
    void calc_init_speed(const GameObject &other);
    void update();
    void cleanup();
};


GameObject GameObject::new_2d_sprite(const char *tex_path)
{
    GameObject g;

    GLfloat vertexData[] =
        {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f};

    GLfloat uvData[] =
        {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f};

    GLuint indexData[] = {0, 1, 2, 3};

    g.m_mesh.init();
    g.m_mesh.attach_buffer(vertexData, 8, GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    g.m_mesh.attach_buffer(uvData, 8, GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    g.m_mesh.attach_buffer(indexData, 4, GL_ELEMENT_ARRAY_BUFFER);

    g.m_texture.load_png(tex_path);

    g.m_position = glm::vec2(0.0f, 0.0f);
    g.m_heading = glm::vec2(0.0f, 0.0f);
    g.m_force = glm::vec2(0.0f, 0.0f);

    g.m_radius = 1.0;
    g.m_mass = 0.0;

    return g;
}



void GameObject::calc_init_speed(const GameObject &other)
{
    float r = m_position.x - other.m_position.x;

    float v = std::sqrt((G * other.m_mass) / r);

    m_heading += glm::vec2(0.0, 1.0) * v;
}

void GameObject::calc_gravity(const GameObject &other)
{
    glm::vec2 dir = other.m_position - this->m_position;
    float r = glm::length(dir);

    dir = dir / r;

    float f = G * ((this->m_mass * other.m_mass) / (r * r));

    this->m_force += dir * f;
}

void GameObject::update()
{
    m_heading += m_force / m_mass;
    m_position += m_heading;

    m_force = glm::vec2(0.f, 0.f);
}

void GameObject::cleanup()
{
    m_mesh.cleanup();
    m_texture.cleanup();
}


struct GameObjectNew
{

    gl_util::Mesh m_mesh;
    gl_util::Texture m_texture;

    glm::vec3 m_position;
	unsigned int m_indexSize;

    GLuint ssb_vertex, ssb_color;

    static GameObjectNew new_cloth();

    void update();
    void cleanup();
};

GameObjectNew GameObjectNew::new_cloth()
{
    GameObjectNew g;


    GLfloat uvData[] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f};

    GLuint indexData[] = { 0, 1, 2, 3 };

    GLfloat colIn[] = { 1.0, 0.0, 0.0, 1.0 };

	unsigned int size = sizeof(vertexDataVec4) / sizeof(vertexDataVec4[0]);

    g.m_mesh.init();
    g.m_mesh.attach_buffer(vertexDataVec4, size, GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    glGenBuffers(1, &g.ssb_vertex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g.ssb_vertex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertexDataVec4), vertexDataVec4, GL_STATIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    glGenBuffers(1, &g.ssb_color);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g.ssb_color);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colIn), colIn, GL_STATIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    //g.m_mesh.attach_buffer(uvData, 8, GL_ARRAY_BUFFER);
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

	g.m_indexSize = sizeof(faceTriIds) / sizeof(faceTriIds[0]);

    g.m_mesh.attach_buffer(faceTriIds, g.m_indexSize, GL_ELEMENT_ARRAY_BUFFER);

    g.m_position = glm::vec3(0.0f, 0.0f, 0.0f);


    return g;
}

void GameObjectNew::update()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssb_vertex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssb_color);
}

void GameObjectNew::cleanup()
{
    m_mesh.cleanup();
    m_texture.cleanup();
}