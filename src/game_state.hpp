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
    bool m_gRenderQuad = true;
    bool m_quit = false;
    bool m_space_down = false;


    std::vector<GameObject> m_game_objects;

    gl_util::Program m_program;

    glm::mat4 m_proj_mat;

    float m_zoom = 4998.0;
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
    m_program = gl_util::Program("./src/shader/vs.glsl", "./src/shader/fs.glsl");

    m_game_objects.push_back(GameObject::new_2d_sprite("./res/sun.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/mercury.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/venus.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/earth.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/moon.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/mars.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/jupiter.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/saturn.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/uranus.png"));
    m_game_objects.push_back(GameObject::new_2d_sprite("./res/neptune.png"));


    m_game_objects[1].m_position.x = 63.81f;
    m_game_objects[2].m_position.x = 107.59f;
    m_game_objects[3].m_position.x = 151.48f;
    m_game_objects[4].m_position.x = 151.48f + 0.38f;
    m_game_objects[5].m_position.x = 248.84f;
    m_game_objects[6].m_position.x = 755.91f;
    m_game_objects[7].m_position.x = 1487.8f;
    m_game_objects[8].m_position.x = 2954.6f;
    m_game_objects[9].m_position.x = 4475.5f;


    m_game_objects[0].m_radius = 696.342f;
    m_game_objects[1].m_radius = 2.44f;
    m_game_objects[2].m_radius = 6.052f;
    m_game_objects[3].m_radius = 6.371f;
    m_game_objects[4].m_radius = 1.8f;
    m_game_objects[5].m_radius = 3.390f;
    m_game_objects[6].m_radius = 69.911f;
    m_game_objects[7].m_radius = 58.232f;
    m_game_objects[8].m_radius = 25.362f;
    m_game_objects[9].m_radius = 24.622f;


    m_game_objects[0].m_mass = 333000.f;
    m_game_objects[1].m_mass = .055f;
    m_game_objects[2].m_mass = .815f;
    m_game_objects[3].m_mass = 1.0f;
    m_game_objects[4].m_mass = .012f;
    m_game_objects[5].m_mass = .107f;
    m_game_objects[6].m_mass = 317.8f;
    m_game_objects[7].m_mass = 95.16f;
    m_game_objects[8].m_mass = 14.54f;
    m_game_objects[9].m_mass = 17.15f;


    for(auto& go: m_game_objects){
        go.m_radius /= 5.0f;
        //go.m_mass *= 1.0f;
    }

    m_game_objects[4].calc_init_speed(m_game_objects[3]);

    for (size_t i = 1; i < m_game_objects.size(); ++i)
        m_game_objects[i].calc_init_speed(m_game_objects[0]);

    float temp = 5000.0f - m_zoom;

    m_proj_mat = glm::ortho(-temp, temp, -temp, temp, -2.0f, 2.0f);
}

void GameState::update()
{
    for (int i_ = 0; i_ < 10000; ++i_) {

        for (size_t i = 1; i < m_game_objects.size(); ++i)
        {
            for (size_t y = 0; y < m_game_objects.size(); ++y)
            {

                if (i == y)
                    continue;

                m_game_objects[i].calc_gravity(m_game_objects[y]);
            }
        }
        
        glm::vec2 temp = m_game_objects[3].m_position;

        for (size_t i = 1; i < m_game_objects.size(); ++i)
        {
            m_game_objects[i].update();
        }

        if(temp.y < 0.0 && m_game_objects[3].m_position.y > 0.0)
            printf("earth took %d calculated seconds to finish an orbit",loops);
        
        loops += 1;
    }


    if (m_game_objects[3].m_position.y == 0.0)
        printf("cycles: %d",loops);


}

void GameState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.1f, 1.f);
    if (m_gRenderQuad)
    {
        m_program.use();

        float temp = 5000.0f - m_zoom;

        glm::vec2 pp = m_game_objects[m_focus].m_position;


        float ratio = static_cast<float>(m_window_height) / static_cast<float>(m_window_width);
        m_proj_mat = glm::ortho(-temp + pp.x, temp + pp.x, (-temp*ratio + pp.y),  (temp*ratio + pp.y), -2.0f, 2.0f);

        for (auto go : m_game_objects)
        {

            glm::mat4 m = glm::translate(glm::vec3(go.m_position.x, go.m_position.y, 0.0f));
            glm::mat4 s = glm::scale(glm::vec3(go.m_radius, go.m_radius, go.m_radius));

            glm::mat4 mvp = m_proj_mat * m * s;

            glUniformMatrix4fv(0, 1, GL_FALSE, &mvp[0][0]);

            go.m_texture.activate_texture(1);
            glUniform1i(1, 1);

            go.m_mesh.bind();
            go.m_mesh.draw_elements(GL_TRIANGLE_FAN, 4);
        }

        glUseProgram(NULL);
    }
}

void GameState::cleanup()
{
    for (auto go : m_game_objects)
    {
        go.cleanup();
    }

    m_program.cleanup();
}

void GameState::handleEvent(SDL_Event event)
{
    // int x = 0, y = 0;
    // SDL_GetMouseState(&x, &y);

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
            if (m_zoom < 4900.0f)
            {
                m_zoom += 100.0f;
            }
        }
        else
        {
            if (m_zoom > 100.0f)
            {
                m_zoom -= 100.0f;
            }
        }

        printf("zoom: %f\n", m_zoom);

        float temp = 5000.0f - m_zoom;

        m_proj_mat = glm::ortho(-temp, temp, -temp, temp, -2.0f, 2.0f);
    }

    break;

    case SDL_KEYDOWN:
    {
        auto key = event.key.keysym.scancode;
        switch (key)
        {
        case SDL_SCANCODE_Q:
            m_gRenderQuad = !m_gRenderQuad;
            break;
        case SDL_SCANCODE_ESCAPE:
            m_quit = true;
            break;
        case SDL_SCANCODE_SPACE:
            m_space_down = true;
            break;
        case SDL_SCANCODE_LEFT:
            if (m_focus > 0)
                m_focus--;
            else
                m_focus = m_game_objects.size() -1;
            break;
        case SDL_SCANCODE_RIGHT:
            if (m_focus < m_game_objects.size() -1 )
                m_focus++;
            else
                m_focus = 0;                       
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

void adj_G(int exp){
    G = 6.65f * powf(10.0,exp);
}

