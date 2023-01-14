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

struct GameState
{
    bool gRenderQuad = true;
    bool m_quit = false;
    bool m_space_down = false;
    bool m_change_vertex = false;

    GameObjectNew m_game_object;

    gl_util::Program m_program;
    gl_util::ComputeShader m_cs;
    gl_util::ComputeShader m_cs_grv;
    gl_util::ComputeShader m_cs_ball;
    gl_util::ComputeShader m_cs_cor;
    gl_util::ComputeShader m_cs_app;
    gl_util::ComputeShader m_cs_post;

    glm::mat4 m_proj_mat;

    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_dir;

    bool m_sphere_active = false;
    glm::vec3 m_sphere_pos;
    float m_sphere_rad;

    bool left = false;
    bool right = false;
    bool forward = false;
    bool backward = false;
    bool up = false;
    bool down = false;

    bool mouse_b_down = false;

    size_t m_focus = 3;

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
    m_program = gl_util::Program("./src/shader/vs.glsl", "./src/shader/fs.glsl", "./src/shader/gs.glsl");

    m_cs = gl_util::ComputeShader("./src/shader/cs.glsl");
    m_cs_grv = gl_util::ComputeShader("./src/shader/c_grav.glsl");
    m_cs_ball = gl_util::ComputeShader("./src/shader/c_ball.glsl");
    m_cs_cor = gl_util::ComputeShader("./src/shader/c_cor.glsl");
    m_cs_app = gl_util::ComputeShader("./src/shader/c_apply.glsl");
    m_cs_post = gl_util::ComputeShader("./src/shader/c_post.glsl");

    m_game_object = GameObjectNew::new_cloth();

    m_cam_pos = glm::vec3(-0.718707, 0.672204, 0.737830);
    m_cam_dir = glm::vec3(0.736265, -0.156785, -0.658441);

    m_sphere_pos = glm::vec3(0.0, 0.3, -0.1);
    m_sphere_rad = 0.2f;
}

void GameState::update()
{
    auto n_vec = rotate(m_cam_dir, 1.57f, glm::vec3(0.0, 1.0, 0.0));
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
        m_cam_pos.y += 0.01;
    }
    if (down)
    {
        m_cam_pos.y -= 0.01;
    }

    m_cs.use();
    m_game_object.update();
    glUniform1i(7, int(m_change_vertex));
    glDispatchCompute((unsigned int)m_game_object.m_verteces, (unsigned int)1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_cs_grv.use();
    m_game_object.update();
    glDispatchCompute((unsigned int)m_game_object.m_verteces, (unsigned int)1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_change_vertex = false;
    for (int i = 0; i < 100; ++i)
    {
        m_cs_cor.use();
        m_game_object.update();
        glDispatchCompute((unsigned int)m_game_object.m_edges, (unsigned int)1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_cs_app.use();
        m_game_object.update();
        glDispatchCompute((unsigned int)m_game_object.m_verteces, (unsigned int)1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        if (m_sphere_active)
        {
            m_cs_ball.use();
            glUniform3fv(0, 1, &m_sphere_pos[0]);
            glUniform1fv(1, 1, &m_sphere_rad);
            m_game_object.update();
            glDispatchCompute((unsigned int)m_game_object.m_verteces, (unsigned int)1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

    }

    m_cs_post.use();
    m_game_object.update();
    glDispatchCompute((unsigned int)m_game_object.m_verteces, (unsigned int)1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void GameState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.1f, 1.f);

    m_program.use();

    float ratio = static_cast<float>(m_window_width) / static_cast<float>(m_window_height);

    // glm::mat4 m = glm::translate(glm::vec3(m_game_object.m_position.x, m_game_object.m_position.y, 0.0f));
    glm::mat4 m = glm::lookAtRH(m_cam_pos, m_cam_pos + m_cam_dir, glm::vec3(0.0, 1.0, 0.0));
    m_proj_mat = glm::perspective(45.0f, ratio, 0.001f, 100.0f);

    glm::mat4 mvp = m_proj_mat * m;

    glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(1, 1, &m_cam_pos[0]);

    // m_game_object.m_texture.activate_texture(1);
    // glUniform1i(1, 1);

    m_game_object.m_mesh.bind();
    m_game_object.m_mesh.draw_elements(GL_TRIANGLES, m_game_object.m_indexSize);
    glBindVertexArray(0);
    glUseProgram(NULL);
}

void GameState::cleanup()
{
    m_game_object.cleanup();
    m_program.cleanup();
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
            std::cout << glm::to_string(m_cam_pos) << std::endl;
            std::cout << glm::to_string(m_cam_dir) << std::endl;
            gRenderQuad = !gRenderQuad;
            if (!gRenderQuad)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case SDL_SCANCODE_E:
            m_change_vertex = true;
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
