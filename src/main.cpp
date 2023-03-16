#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>

#include <stdio.h>
#include <string>
#include <vector>
#include <random>

#include <algorithm>

#include "opengl_util.hpp"
#include "settings.hpp"

bool init_sdl();
void close();
void buffer_data();
void sort_double_bubble();

SDL_Window *gWindow = NULL;
SDL_GLContext gContext;

#define WORKGROUPS 512
size_t elements = WORKGROUPS * 26;
std::vector<int32_t> v;
GLuint buffer = 0;
gl_util::ComputeShader double_bubble;

int main(int argc, char *args[])
{
	if (!init_sdl())
	{
		return -1;
	}

	double_bubble = gl_util::ComputeShader("./src/shader/double_bubble.glsl");
	buffer_data();

	SDL_Event event;
	bool running = true;
	while (running)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
				running = false;

			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_SPACE:
					sort_double_bubble();
					printf("space\n");

					break;
				case SDLK_ESCAPE:
					running = false;

					break;
				default:
					break;
				}
			}
		}

		SDL_GL_SwapWindow(gWindow);
	}

	close();
	return 0;
}

void sort_double_bubble()
{
	double_bubble.use();
	glUniform1ui(0, elements);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);

	size_t passes = elements / (WORKGROUPS *2);
	for (size_t j = 0; j < passes; ++j)
	{
		if (j % 2 == 0)
		{
			glUniform1ui(1, 0);
			glDispatchCompute(passes, 1, 1);
		}
		else
		{
			glUniform1ui(1, 1);
			glDispatchCompute(passes - 1, 1, 1);
		}

		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	
	void *data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	int *stuff = static_cast<int *>(data);

	for (size_t i = 0; i < v.size(); ++i)
	{
		if (stuff[i] != v[i])
		{
			printf("not sorted s:%d v:%d", stuff[i], v[i]);
			break;
		}
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void buffer_data()
{
	v.reserve(elements);

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, elements); // distribution in range [1, 6]

	for (int i = 0; i < elements; ++i)
	{
		v.push_back(dist6(rng));
	}

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int32_t) * v.size(), &v[0], GL_DYNAMIC_COPY);

	sort(v.begin(), v.end());
}

bool init_sdl()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_WindowFlags w_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		gWindow = SDL_CreateWindow("Cloth Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, w_flags);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
				{
					printf("Failed to initialize GLAD");
					return false;
				}

				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				// glEnable(GL_DEBUG_OUTPUT);
				// glDebugMessageCallback(MessageCallback, 0);
			}
		}
	}

	return success;
}

void close()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}