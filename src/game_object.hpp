#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "opengl_util.hpp"
#include "settings.hpp"

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
