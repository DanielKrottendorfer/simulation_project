#pragma once

#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
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
    gl_util::ComputeShader m_cs_cor;
    gl_util::ComputeShader m_cs_app;

    glm::mat4 m_proj_mat;

    float m_camera_zoom = 0.5f;
    float m_camera_x = 0.0f;
    float m_camera_y = 0.76f;

    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

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
    m_program = gl_util::Program("./src/shader/vs.glsl", "./src/shader/fs.glsl");

    m_cs = gl_util::ComputeShader("./src/shader/cs.glsl");
    m_cs_grv = gl_util::ComputeShader("./src/shader/c_grav.glsl");
    m_cs_cor = gl_util::ComputeShader("./src/shader/c_cor.glsl");
    m_cs_app = gl_util::ComputeShader("./src/shader/c_apply.glsl");

    m_game_object = GameObjectNew::new_cloth();

    float temp = m_camera_zoom;

    m_proj_mat = glm::ortho(-temp, temp, -temp, temp, -2.0f, 2.0f);
}

void GameState::update()
{
    if (left)
    {
        m_camera_x -= 0.01f;
    }
    if (right)
    {
        m_camera_x += 0.01f;
    }
    if (up)
    {
        m_camera_y += 0.01f;
    }
    if (down)
    {
        m_camera_y -= 0.01f;
    }

    std::cout << m_camera_y << std::endl;
    std::cout << m_camera_zoom << std::endl;

     m_cs.use();
     m_game_object.update();
     glUniform1i(7, int(m_change_vertex));
     glDispatchCompute((unsigned int)100, (unsigned int)1, 1);
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
    }
}

void GameState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.1f, 1.f);

    m_program.use();

    float ratio = static_cast<float>(m_window_height) / static_cast<float>(m_window_width);

    glm::mat4 m = glm::translate(glm::vec3(m_game_object.m_position.x, m_game_object.m_position.y, 0.0f));

    m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);

    glm::mat4 mvp = m_proj_mat * m;

    glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);

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

    case SDL_MOUSEWHEEL:
    {
        auto mouse_wheel_dir = event.wheel.y;

        if (mouse_wheel_dir > 0.0f)
        {
            m_camera_zoom -= 0.05f;
        }
        else
        {
            m_camera_zoom += 0.05f;
        }

        // printf("zoom: %f\n", m_camera_zoom);
    }
    break;

    case SDL_MOUSEMOTION:
    {
        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);

        // std::cout << "X MOTION: " << x << std::endl;
        // std::cout << "Y MOTION: " << y << std::endl;
    }
    break;

    case SDL_KEYDOWN:
    {
        auto key = event.key.keysym.scancode;
        switch (key)
        {
        case SDL_SCANCODE_Q:
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
            // m_camera_y += 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
            up = true;
            break;
        case SDL_SCANCODE_A:
            left = true;
            // m_camera_x -= 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
            break;
        case SDL_SCANCODE_S:
            down = true;
            // m_camera_y -= 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
            break;
        case SDL_SCANCODE_D:
            right = true;
            // m_camera_x += 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
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
            up = false;
            // m_camera_y += 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
            break;
        case SDL_SCANCODE_A:
            left = false;
            // m_camera_x -= 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
            break;
        case SDL_SCANCODE_S:
            down = false;
            // m_camera_y -= 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
            break;
        case SDL_SCANCODE_D:
            right = false;
            // m_camera_x += 0.1f;
            // m_proj_mat = glm::ortho(-m_camera_zoom + m_camera_x, m_camera_zoom + m_camera_x, -m_camera_zoom + m_camera_y, m_camera_zoom + m_camera_y, -2.0f, 2.0f);
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
