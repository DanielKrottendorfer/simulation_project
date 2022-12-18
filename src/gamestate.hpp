#pragma once


#include <iostream>
#include <glad/glad.h>
#include <vector>

#include "gl_util.hpp"
#include "gamestate.hpp"

struct GameState{
    int a = 5;
    bool gRenderQuad = true;

    gl_util::Mesh m_mesh;
    gl_util::Program m_program;

    void handleKeys(unsigned char key, int x, int y);
    void update();
    void render();

    GameState();
};

GameState::GameState() {

	m_program = gl_util::Program("./src/shader/vs.glsl", "./src/shader/fs.glsl");

	glClearColor(0.f, 0.f, 0.f, 1.f);

	GLfloat vertexData[] =
	{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f };
	GLuint indexData[] = { 0, 1, 2, 3 };

	m_mesh = gl_util::Mesh::new_mesh();

	m_mesh.attach_buffer(vertexData, 8, GL_ARRAY_BUFFER);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	m_mesh.attach_buffer(indexData, 4, GL_ELEMENT_ARRAY_BUFFER);

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
	glClear(GL_COLOR_BUFFER_BIT);

	if (gRenderQuad)
	{
        m_program.use();

		m_mesh.bind();
		m_mesh.draw_elements(GL_TRIANGLE_FAN, 4);

		glUseProgram(NULL);
	}
}