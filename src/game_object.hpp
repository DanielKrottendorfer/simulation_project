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

    std::vector<Edge> edgeIdVector;
    std::vector<GLfloat> colorVec;

    std::vector<std::array<int,4>> edg_refs;
    std::vector<int> edg_ref_lens;

    for(int x = 0; x < ((sizeof(vertexDataVec4) / sizeof(vertexDataVec4[0])) - 4); x += 4)
    {
        for (int y = x + 4; y < sizeof(vertexDataVec4) / sizeof(vertexDataVec4[0]); y += 4)
        {
            glm::vec2 va(vertexDataVec4[x], vertexDataVec4[x + 1]);
            glm::vec2 vb(vertexDataVec4[y], vertexDataVec4[y + 1]);
            
            float t = glm::length((va - vb));
            
            if (t <= 0.011) {
                Edge temp;
                temp.edgeA = x / 4;
                temp.edgeB = y / 4;
                edgeIdVector.push_back(temp);
            }

        }
        colorVec.push_back(1.0f);
        colorVec.push_back(0.0f);
        colorVec.push_back(1.0f);
        colorVec.push_back(1.0f);
        
        std::array<int, 4> ab {{0,0,0,0}};
        edg_refs.push_back(ab);
        edg_ref_lens.push_back(0);
    }
    colorVec.push_back(1.0f);
    colorVec.push_back(0.0f);
    colorVec.push_back(1.0f);
    colorVec.push_back(1.0f);

    std::array<int, 4> a = { 0,0,0,0 };
    edg_refs.push_back(a);
    edg_ref_lens.push_back(0);


    for (int i=0 ; i<edgeIdVector.size() ; ++i){
        Edge edg = edgeIdVector[i];

        int i1 = edg.edgeA;
        edg_refs[i1][edg_ref_lens[i1]] = i;
        edg_ref_lens[i1] += 1;

        int i2 = edg.edgeB;
        edg_refs[i2][edg_ref_lens[i2]] = i;
        edg_ref_lens[i2] += 1;
    }


	unsigned int size = sizeof(vertexDataVec4) / sizeof(vertexDataVec4[0]);

    GLfloat* v_data = vertexDataVec4;
    std::cout << edgeIdVector.size() << std::endl;

    g.m_mesh.init();
    g.m_mesh.attach_buffer(v_data, size, GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    g.m_mesh.attach_buffer(&colorVec[0], size, GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
        

    g.m_mesh.attach_buffer(&edgeIdVector[0], edgeIdVector.size(), GL_ARRAY_BUFFER);
    g.m_mesh.attach_buffer<glm::vec4>(NULL, edgeIdVector.size(), GL_ARRAY_BUFFER);
    g.m_mesh.attach_buffer(&edg_refs[0], edg_refs.size(), GL_ARRAY_BUFFER);
    g.m_mesh.attach_buffer(&edg_ref_lens[0], edg_ref_lens.size(), GL_ARRAY_BUFFER);


	g.m_indexSize = sizeof(faceTriIds) / sizeof(faceTriIds[0]);
    g.m_mesh.attach_buffer(faceTriIds, g.m_indexSize, GL_ELEMENT_ARRAY_BUFFER);

    g.m_position = glm::vec3(0.0f, 0.0f, 0.0f);


    return g;
}

void GameObjectNew::update()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_mesh.mVBOs[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_mesh.mVBOs[1]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_mesh.mVBOs[2]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_mesh.mVBOs[3]);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_mesh.mVBOs[4]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_mesh.mVBOs[5]);
}

void GameObjectNew::cleanup()
{
    m_mesh.cleanup();
    m_texture.cleanup();
}