#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <string>

#include "opengl_util.hpp"
#include "game_state.hpp"

bool init();
void close();

SDL_Window *gWindow = NULL;
SDL_GLContext gContext;
void MessageCallback(GLenum source,
					 GLenum type,
					 GLuint id,
					 GLenum severity,
					 GLsizei length,
					 const GLchar *message,
					 const void *userParam)
{
	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		   (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		   type, severity, message);
}

bool init()
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

		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef DEBUG_CALLBACK
				glEnable(GL_DEBUG_OUTPUT);
				glDebugMessageCallback(MessageCallback, 0);
#endif
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

int main(int argc, char *args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{

		GameState g = GameState();

		bool quit = false;

		SDL_Event e;

		SDL_StartTextInput();

		while (!g.m_quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				g.handleEvent(e);
			}

			g.update();
			g.render();

			SDL_GL_SwapWindow(gWindow);
		}

		g.cleanup();

		SDL_StopTextInput();
	}

	close();
	return 0;
}