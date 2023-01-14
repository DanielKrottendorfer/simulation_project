#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include "opengl_util.hpp"
#include "settings.hpp"
#include "vertex_data.hpp"
#include <array>

struct Edge
{
    int edgeA;
    int edgeB;
};
struct EdgeRefs
{
    int refA = 0;
    int refB = 0;
    int refC = 0;
    int refD = 0;
};

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

    int m_edges;
    int m_verteces;

    static GameObjectNew new_cloth();

    void bind_shader_storage_buffer();
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

    std::vector<std::array<int, 2>> edgeIdVector;
    std::vector<std::array<int, 4>> edg_refs;
    std::vector<int> edg_ref_lens;
    std::vector<int> stiff;

    unsigned int v_size = sizeof(vertexDataVec4) / sizeof(vertexDataVec4[0]);
    for (int x = 0; x < (v_size - 4); x += 4)
    {
        for (int y = x + 4; y < v_size; y += 4)
        {
            glm::vec2 va(vertexDataVec4[x], vertexDataVec4[x + 1]);
            glm::vec2 vb(vertexDataVec4[y], vertexDataVec4[y + 1]);

            float t = glm::length((va - vb));

            if (t <= 0.013)
            {
                edgeIdVector.push_back({x / 4, y / 4});
            }
        }

        std::array<int, 4> ab{{0, 0, 0, 0}};
        edg_refs.push_back(ab);
        edg_ref_lens.push_back(0);
    }

    std::array<int, 4> a = {0, 0, 0, 0};
    edg_refs.push_back(a);
    edg_ref_lens.push_back(0);

    float y_max = vertexDataVec4[1];
    float x_max = vertexDataVec4[0];
    float x_min = vertexDataVec4[0];

    for (int i = 0; i < v_size; i += 4)
    {
        if (y_max < vertexDataVec4[i + 1])
        {
            y_max = vertexDataVec4[i + 1];
        }
        if (x_max < vertexDataVec4[i])
        {
            x_max = vertexDataVec4[i];
        }
        if (x_min > vertexDataVec4[i])
        {
            x_min = vertexDataVec4[i];
        }
    }

    for (int i = 0; i < v_size; i += 4)
    {
        if (y_max == vertexDataVec4[i + 1] && (vertexDataVec4[i] == x_max || vertexDataVec4[i] == x_min))
        {
            stiff.push_back(1);
        }
        else
        {
            stiff.push_back(0);
        }
    }

    for (int i = 0; i < edgeIdVector.size(); ++i)
    {
        int i1 = edgeIdVector[i][0];
        edg_refs[i1][edg_ref_lens[i1]] = i;
        edg_ref_lens[i1] += 1;

        int i2 = edgeIdVector[i][1];
        edg_refs[i2][edg_ref_lens[i2]] = i;
        edg_ref_lens[i2] += 1;
    }

    GLfloat *v_data = vertexDataVec4;
    std::cout << edgeIdVector.size() << std::endl;

    g.m_edges = edgeIdVector.size();
    g.m_verteces = v_size / 4;

    g.m_mesh.init();
    g.m_mesh.attach_buffer(v_data, v_size, GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    g.m_mesh.attach_buffer(&edgeIdVector[0], edgeIdVector.size(), GL_ARRAY_BUFFER);

    // correction vector
    g.m_mesh.attach_buffer<glm::vec4>(NULL, edgeIdVector.size(), GL_ARRAY_BUFFER);
    g.m_mesh.attach_buffer(&edg_refs[0], edg_refs.size(), GL_ARRAY_BUFFER);
    g.m_mesh.attach_buffer(&edg_ref_lens[0], edg_ref_lens.size(), GL_ARRAY_BUFFER);
    g.m_mesh.attach_buffer(&stiff[0], stiff.size(), GL_ARRAY_BUFFER);
    // velocity vector
    g.m_mesh.attach_buffer<glm::vec4>(NULL, v_size / 4, GL_ARRAY_BUFFER);
    // vertex temp vector
    g.m_mesh.attach_buffer<glm::vec4>(NULL, v_size / 4, GL_ARRAY_BUFFER);


    // element buffer
    g.m_indexSize = sizeof(faceTriIds) / sizeof(faceTriIds[0]);
    g.m_mesh.attach_buffer(faceTriIds, g.m_indexSize, GL_ELEMENT_ARRAY_BUFFER);


    g.m_position = glm::vec3(0.0f, 0.0f, 0.0f);

    return g;
}

void GameObjectNew::bind_shader_storage_buffer()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_mesh.mVBOs[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_mesh.mVBOs[1]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_mesh.mVBOs[2]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_mesh.mVBOs[3]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_mesh.mVBOs[4]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, m_mesh.mVBOs[5]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, m_mesh.mVBOs[6]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, m_mesh.mVBOs[7]);
}

void GameObjectNew::cleanup()
{
    m_mesh.cleanup();
    m_texture.cleanup();
}