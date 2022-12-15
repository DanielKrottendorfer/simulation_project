// Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>

#include "gl_util.h"

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Starts up SDL, creates window, and initializes OpenGL
bool init();

// Initializes rendering program and clear color
bool initGL();

// Input handler
void handleKeys(unsigned char key, int x, int y);

// Per frame update
void update();

// Renders quad to the screen
void render();

// Frees media and shuts down SDL
void close();

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// OpenGL context
SDL_GLContext gContext;

// Render flag
bool gRenderQuad = true;

// Graphics program
GLuint program = 0;

gl_util::Mesh *mesh = NULL;

bool init()
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		SDL_GL_LoadLibrary(NULL); // Default OpenGL is fine.

		// Request an OpenGL 4.5 context (should be core)
		// SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		// Also request a depth buffer
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		// Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize GLAD
				if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
				{
					printf("Failed to initialize GLAD");
					return false;
				}

				// Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				// Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
		}
	}

	return success;
}

bool initGL()
{
	program = glCreateProgram();

	GLint vs = gl_util::new_shader("./src/shader/vs.glsl", GL_VERTEX_SHADER);
	glAttachShader(program, vs);
	GLint fs = gl_util::new_shader("./src/shader/fs.glsl", GL_FRAGMENT_SHADER);
	glAttachShader(program, fs);

	gl_util::link_shader(program);

	// Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// VBO data
	GLfloat vertexData[] =
		{
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f,
			-0.5f, 0.5f};

	// IBO data
	GLuint indexData[] = {0, 1, 2, 3};

	mesh = new gl_util::Mesh();

	mesh->attach_buffer(vertexData, 8, GL_ARRAY_BUFFER);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	mesh->attach_buffer(indexData, 4, GL_ELEMENT_ARRAY_BUFFER);

	return true;
}

void handleKeys(unsigned char key, int x, int y)
{
	// Toggle quad
	if (key == 'q')
	{
		gRenderQuad = !gRenderQuad;
	}
}

void update()
{
	// No per frame update needed
}

void render()
{
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Render quad
	if (gRenderQuad)
	{
		// Bind program
		glUseProgram(program);

		mesh->bind();
		mesh->draw_elements(GL_TRIANGLE_FAN, 4);

		// Unbind program
		glUseProgram(NULL);
	}
}

void close()
{
	// Deallocate program
	glDeleteProgram(program);

	// Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char *args[])
{
	// Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		// Main loop flag
		bool quit = false;

		// Event handler
		SDL_Event e;

		// Enable text input
		SDL_StartTextInput();

		// While application is running
		while (!quit)
		{
			// Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				// User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				// Handle keypress with current mouse position
				else if (e.type == SDL_TEXTINPUT)
				{
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					handleKeys(e.text.text[0], x, y);
				}
			}

			// Render quad
			render();

			// Update screen
			SDL_GL_SwapWindow(gWindow);
		}

		// Disable text input
		SDL_StopTextInput();
	}

	// Free resources and close SDL
	close();
	mesh->~Mesh();
	return 0;
}