#pragma once

#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "opengl_util.hpp"
#include "game_object.hpp"

struct GameState
{
    int a = 5;
    bool gRenderQuad = true;

    GameObject m_earth;

    gl_util::Program m_program;

    bool m_quit = false;

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
}

void GameState::handleEvent(SDL_Event event)
{

    if (event.type == SDL_QUIT)
    {

        m_quit = true;
    }
    else if (event.type == SDL_TEXTINPUT)
    {

        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        handleKeys(event.text.text[0], x, y);
    }
    else if (event.type == SDL_KEYDOWN)
    {

        auto key = event.key;

        switch (key.keysym.scancode)
        {
        case SDL_SCANCODE_ESCAPE:
            m_quit = true;
            break;

        default:
            break;
        }
    }
}

void GameState::handleKeys(unsigned char key, int x, int y)
{
    if (key == 'q')
    {
        printf("q");
        gRenderQuad = !gRenderQuad;
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

        m_earth.m_texture.activate_texture(0);

        glUniform1i(0, 0);

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
