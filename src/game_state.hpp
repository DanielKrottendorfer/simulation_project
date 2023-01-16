#pragma once

#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL.h>

#include "opengl_util.hpp"
#include "game_object.hpp"
#include "settings.hpp"
#include "obj_loader.hpp"

struct GameState
{
    bool m_gRenderQuad = true;
    bool m_quit = false;
    bool m_space_down = false;
    bool m_pull_vertex = false;

    GameObject m_cloth;
    gl_util::Mesh m_sphere;
    int m_sphere_vert_c = 0;

    gl_util::Program m_cloth_render_program;
    gl_util::Program m_sphere_render_program;
    gl_util::ComputeShader m_cs_pull;
    gl_util::ComputeShader m_cs_grvity;
    gl_util::ComputeShader m_cs_sphere;
    gl_util::ComputeShader m_cs_correction;
    gl_util::ComputeShader m_cs_apply_cor;
    gl_util::ComputeShader m_cs_post;

    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_dir;

    bool m_sphere_active = false;
    glm::vec3 m_sphere_pos;
    float m_sphere_rad;

    int m_simulation_steps = 1000;

    bool left = false;
    bool right = false;
    bool forward = false;
    bool backward = false;
    bool up = false;
    bool down = false;
    bool mouse_b_down = false;

    size_t m_focus = 3;

    int loops = 0;

    GLsizei m_window_width = SCREEN_WIDTH;
    GLsizei m_window_height = SCREEN_HEIGHT;

    void handleKeys(unsigned char key, int x, int y);
    void handleEvent(SDL_Event event);
    void update();
    void render();

    void cleanup();

    GameState();
};

GameState::GameState()
{
    m_cloth_render_program = gl_util::Program("./src/shader/vs.glsl", "./src/shader/fs.glsl", "./src/shader/gs.glsl");
    m_sphere_render_program = gl_util::Program("./src/shader/vs_sphere.glsl", "./src/shader/fs_sphere.glsl");

    m_cs_pull = gl_util::ComputeShader("./src/shader/c_pull.glsl");
    m_cs_grvity = gl_util::ComputeShader("./src/shader/c_gravity.glsl");
    m_cs_sphere = gl_util::ComputeShader("./src/shader/c_sphere.glsl");
    m_cs_correction = gl_util::ComputeShader("./src/shader/c_correction.glsl");
    m_cs_apply_cor = gl_util::ComputeShader("./src/shader/c_apply.glsl");
    m_cs_post = gl_util::ComputeShader("./src/shader/c_post.glsl");

    m_cloth = GameObject::new_cloth();

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec3> out_normals;

    if (!loadOBJ("./res/sphere.obj", out_vertices, out_normals))
    {
        std::cout << "ERROR: model did not load" << std::endl;
    }

    m_sphere.init();
    m_sphere.attach_buffer(&out_vertices[0], out_vertices.size(), GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    m_sphere.attach_buffer(&out_normals[0], out_vertices.size(), GL_ARRAY_BUFFER);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
    m_sphere_vert_c = out_vertices.size();

    m_sphere_pos = glm::vec3(0.0, 0.7, -0.4);
    m_sphere_rad = 0.1f;

    m_cam_pos = glm::vec3(0.467810, 0.854653, 0.993788);
    m_cam_dir = glm::vec3(-0.322434, -0.179466, -0.929587);
}

void GameState::update()
{
    auto n_vec = rotate(m_cam_dir, 1.57f, glm::vec3(0.0, 1.0, 0.0));
    n_vec.y = 0.0;
    if (left)
    {
        m_cam_pos += n_vec * 0.01f;
    }
    if (right)
    {
        m_cam_pos -= n_vec * 0.01f;
    }
    if (forward)
    {
        m_cam_pos += m_cam_dir * 0.01f;
    }
    if (backward)
    {
        m_cam_pos -= m_cam_dir * 0.01f;
    }
    if (up)
    {
        m_cam_pos.y += 0.01f;
    }
    if (down)
    {
        m_cam_pos.y -= 0.01f;
    }

    if (m_pull_vertex)
    {
        m_cs_pull.use();
        m_cloth.bind_shader_storage_buffer();
        glDispatchCompute(m_cloth.m_verteces, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        m_pull_vertex = false;
    }

    m_cs_grvity.use();
    m_cloth.bind_shader_storage_buffer();
    glDispatchCompute((m_cloth.m_verteces / 64) + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    for (int i = 0; i < m_simulation_steps; ++i)
    {
        m_cs_correction.use();
        m_cloth.bind_shader_storage_buffer();
        glDispatchCompute((m_cloth.m_edges / 64) + 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_cs_apply_cor.use();
        m_cloth.bind_shader_storage_buffer();
        glDispatchCompute((m_cloth.m_verteces / 64) + 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        if (m_sphere_active)
        {
            m_cs_sphere.use();
            glUniform3fv(0, 1, &m_sphere_pos[0]);
            glUniform1fv(1, 1, &m_sphere_rad);
            m_cloth.bind_shader_storage_buffer();
            glDispatchCompute((m_cloth.m_verteces / 64) + 1, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
        
        loops += 1;
    }

    m_cs_post.use();
    m_cloth.bind_shader_storage_buffer();
    glDispatchCompute((m_cloth.m_verteces / 64) + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void GameState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.1f, 1.f);

    m_cloth_render_program.use();

    float ratio = static_cast<float>(m_window_width) / static_cast<float>(m_window_height);

    glm::mat4 view = glm::lookAtRH(m_cam_pos, m_cam_pos + m_cam_dir, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 proj = glm::perspective(45.0f, ratio, 0.001f, 100.0f);

    glm::mat4 vp = proj * view;

    glUniformMatrix4fv(0, 1, GL_FALSE, &vp[0][0]);
    glUniform3fv(1, 1, &m_cam_pos[0]);

    m_cloth.m_mesh.bind();
    m_cloth.m_mesh.draw_elements(GL_TRIANGLES, m_cloth.m_indexSize);

    if (m_sphere_active)
    {
        m_sphere_render_program.use();

        vp = vp * glm::translate(m_sphere_pos) * glm::scale(glm::vec3(m_sphere_rad, m_sphere_rad, m_sphere_rad));

        glUniformMatrix4fv(0, 1, GL_FALSE, &vp[0][0]);
        glUniform3fv(1, 1, &m_cam_pos[0]);

        m_sphere.bind();
        m_sphere.draw_arrays(GL_TRIANGLES, m_sphere_vert_c);
    }

    glBindVertexArray(0);
    glUseProgram(NULL);
}

void GameState::cleanup()
{
    m_cloth.cleanup();
    m_sphere.cleanup();

    m_cloth_render_program.cleanup();
    m_sphere_render_program.cleanup();

    m_cs_pull.cleanup();
    m_cs_grvity.cleanup();
    m_cs_sphere.cleanup();
    m_cs_correction.cleanup();
    m_cs_apply_cor.cleanup();
    m_cs_post.cleanup();
}

void GameState::handleEvent(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_QUIT:
    {
        m_quit = true;
    }
    break;

    case SDL_WINDOWEVENT:
    {
        auto window = event.window;
        if (window.event == SDL_WINDOWEVENT_RESIZED)
        {
            GLsizei width = window.data1;
            GLsizei height = window.data2;
            glViewport(0, 0, width, height);
            m_window_width = width;
            m_window_height = height;
        }
    }
    break;

    case SDL_MOUSEBUTTONDOWN:
    {
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
            mouse_b_down = true;
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }
    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
            mouse_b_down = false;
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }
    break;

    case SDL_MOUSEMOTION:
    {
        if (mouse_b_down)
        {
            float x = static_cast<float>(event.motion.xrel);
            m_cam_dir = rotate(m_cam_dir, -0.001f * x, glm::vec3(0.0, 1.0, 0.0));

            auto n_vec = rotate(m_cam_dir, 1.57f, glm::vec3(0.0, 1.0, 0.0));
            float y = static_cast<float>(event.motion.yrel);
            m_cam_dir = rotate(m_cam_dir, 0.001f * y, glm::vec3(n_vec.x, 0.0, n_vec.z));
        }
    }
    break;

    case SDL_KEYDOWN:
    {
        auto key = event.key.keysym.scancode;
        switch (key)
        {
        case SDL_SCANCODE_Q:
            m_gRenderQuad = !m_gRenderQuad;
            if (!m_gRenderQuad)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case SDL_SCANCODE_E:
            m_pull_vertex = true;
            break;
        case SDL_SCANCODE_ESCAPE:
            m_quit = true;
            break;
        case SDL_SCANCODE_W:
            forward = true;
            break;
        case SDL_SCANCODE_A:
            left = true;
            break;
        case SDL_SCANCODE_S:
            backward = true;
            break;
        case SDL_SCANCODE_D:
            right = true;
            break;
        case SDL_SCANCODE_Z:
            up = true;
            break;
        case SDL_SCANCODE_X:
            down = true;
            break;
        default:
            break;
        }
    }
    break;

    case SDL_KEYUP:
    {
        auto key = event.key.keysym.scancode;
        switch (key)
        {
        case SDL_SCANCODE_W:
            forward = false;
            break;
        case SDL_SCANCODE_A:
            left = false;
            break;
        case SDL_SCANCODE_S:
            backward = false;
            break;
        case SDL_SCANCODE_D:
            right = false;
            break;
        case SDL_SCANCODE_Z:
            up = false;
            break;
        case SDL_SCANCODE_X:
            down = false;
            break;
        default:
            break;
        }
    }
    break;

    default:
        break;
    }
}
