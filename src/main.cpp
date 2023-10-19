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
void sort_bitonic_merge();
void check_sorted();

SDL_Window *gWindow = NULL;
SDL_GLContext gContext;

#define WORKGROUPS 1024
size_t elements = WORKGROUPS * 2;
std::vector<int32_t> v;
GLuint buffer = 0;
gl_util::ComputeShader double_bubble;
gl_util::ComputeShader bitonic_merge;

int main(int argc, char *args[])
{
	if (!init_sdl())
	{
		return -1;
	}

	double_bubble = gl_util::ComputeShader("./src/shader/double_bubble.glsl");
	bitonic_merge = gl_util::ComputeShader("./src/shader/bitonic_merge.glsl");
	buffer_data();

	SDL_Event event;
	bool running = true;

	GLuint query = 0;
	glGenQueries(1, &query);

	GLint temp = 0;
	float time = 0.0f;

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

					temp = 0;
					glGetQueryObjectiv(query, GL_QUERY_RESULT_NO_WAIT, &temp);
					if (temp > 0)
					{
						time = static_cast<float>(temp);
						printf("ms: %f\n", time / 1000000.0f);
					}

					glBeginQuery(GL_TIME_ELAPSED, query);
					sort_double_bubble();
					glEndQuery(GL_TIME_ELAPSED);
					printf("space\n");

					check_sorted();

					break;
				case SDLK_m:

					temp = 0;
					glGetQueryObjectiv(query, GL_QUERY_RESULT_NO_WAIT, &temp);
					if (temp > 0)
					{
						time = static_cast<float>(temp);
						printf("ms: %f\n", time / 1000000.0f);
					}

					glBeginQuery(GL_TIME_ELAPSED, query);
					sort_bitonic_merge();
					glEndQuery(GL_TIME_ELAPSED);
					printf("m\n");

					check_sorted();

					break;
				case SDLK_ESCAPE:
					running = false;

					break;
				case SDLK_a:
					GLint maximum_shared_mem_size;
					glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &maximum_shared_mem_size);
					printf("%d", maximum_shared_mem_size);
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

	size_t passes = elements / (WORKGROUPS * 2);
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
}

void sort_bitonic_merge()
{
	const uint32_t n = static_cast<uint32_t>(elements);
	const uint32_t workgroup_count = n / (WORKGROUPS * 2);

	uint32_t h = WORKGROUPS * 2;

	// local_bms( n, h );
	bitonic_merge.use();
	glUniform1ui(0, h);
	glUniform1ui(1, 0);
	glDispatchCompute(workgroup_count, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// we must now double h, as this happens before every flip
	h *= 2;

	for (; h <= n; h *= 2)
	{

		// big_flip( n, h );
		glUniform1ui(0, h);
		glUniform1ui(1, 1);
		glDispatchCompute(workgroup_count, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		for (uint32_t hh = h / 2; hh > 1; hh /= 2)
		{

			if (hh <= WORKGROUPS * 2)
			{
				// local_disperse( n, hh );
				glUniform1ui(0, hh);
				glUniform1ui(1, 2);
				glDispatchCompute(workgroup_count, 1, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
				break;
			}
			else
			{
				// big_disperse( n, hh );
				glUniform1ui(0, hh);
				glUniform1ui(1, 3);
				glDispatchCompute(workgroup_count, 1, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			}
		}
	}
}

void check_sorted()
{

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

	void *data = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	int *stuff = static_cast<int *>(data);

	for (size_t i = 0; i < v.size() - 1; ++i)
	{
		if (stuff[i] > stuff[i + 1])
		{
			printf("not sorted s:%d s1:%d i:%d\n", stuff[i], stuff[i + 1], i);
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
				// glDebugMessageCallback(gl_util::MessageCallback, 0);
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