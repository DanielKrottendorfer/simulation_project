#pragma once

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <glm/ext.hpp>

#include "opengl_util.hpp"
#include "game_object.hpp"

struct GameState
{
    int a = 5;
    bool gRenderQuad = true;
    bool m_quit = false;

    GameObject m_earth;

    gl_util::Program m_program;

    glm::mat4 m_proj_mat;

    

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

    m_earth = GameObject::new_2d_sprite("./res/earth.png");

    m_proj_mat = glm::ortho( -1, 1, -1, 1, -1, 1 );
}

void GameState::handleEvent(SDL_Event event)
{
    // int x = 0, y = 0;
    // SDL_GetMouseState(&x, &y);

    if (event.type == SDL_QUIT)
    {
        m_quit = true;
    }else if (event.type == SDL_WINDOWEVENT){

        auto window = event.window;
        if (window.event == SDL_WINDOWEVENT_RESIZED){
            GLsizei width = window.data1;
            GLsizei height = window.data2;
            glViewport(0,0,width,height);
        }

    }
    else if (event.type == SDL_KEYDOWN)
    {
        auto key = event.key.keysym.scancode;

        switch (key)
        {
        case SDL_SCANCODE_Q:
            gRenderQuad = !gRenderQuad;
            break;
        case SDL_SCANCODE_ESCAPE:
            m_quit = true;
            break;
        default:
            break;
        }
    }
}

void GameState::update()
{
}

void GameState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 1.f, 1.f, 1.f);
    if (gRenderQuad)
    {
        m_program.use();

        glUniformMatrix4fv(0,1,GL_FALSE,&m_proj_mat[0][0]);

        m_earth.m_texture.activate_texture(1);
        glUniform1i(1, 1);
        
        m_earth.m_mesh.bind();
        m_earth.m_mesh.draw_elements(GL_TRIANGLE_FAN, 4);

        glUseProgram(NULL);
    }
}

void GameState::cleanup()
{
    m_earth.cleanup();

    m_program.cleanup();
}
