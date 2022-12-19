#pragma once

#include "opengl_util.hpp"

#include <glm/glm.hpp>

struct GameObject
{

    gl_util::Mesh m_mesh;
    gl_util::Texture m_texture;

    glm::vec2 m_position;
    glm::vec2 m_heading;
    float m_speed;

    static GameObject new_2d_sprite(const char *tex_path);
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

    return g;
}

void GameObject::cleanup()
{
    m_mesh.cleanup();
    m_texture.cleanup();
}
