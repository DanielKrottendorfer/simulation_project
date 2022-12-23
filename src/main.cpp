#include <SDL.h>
#include <glad/glad.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <chrono>
#include <thread>
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

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		SDL_WindowFlags w_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, w_flags);
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

		GameState game_state = GameState();

		
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
    	const char* glsl_version = "#version 330";
		ImGui_ImplOpenGL3_Init(glsl_version);


		bool quit = false;

		SDL_Event event;
		SDL_StartTextInput();

		ImVec2 window_pos = ImVec2(0.0f,0.0f);

		int zoom = game_state.m_zoom;
		int delay = 100;
		int g_exp = -11;

		adj_G(g_exp);

		while (!game_state.m_quit)
		{
			while (SDL_PollEvent(&event) != 0)
			{
				
            	ImGui_ImplSDL2_ProcessEvent(&event);
				game_state.handleEvent(event);
			}


			game_state.update();
			game_state.render();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();
			
			zoom = game_state.m_zoom;
			ImGui::Begin("Controls");
			ImGui::SetWindowPos(window_pos);
			ImGui::SliderInt("zoom",&zoom,1,4998);
			game_state.m_zoom = zoom;
			ImGui::SliderInt("delay",&delay,0,100);
			if (ImGui::SliderInt("g", &g_exp, -11, 0)) {
				adj_G(g_exp);
			};
			ImGui::End();

        	ImGui::Render();

        	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			SDL_GL_SwapWindow(gWindow);
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		game_state.cleanup();

		SDL_StopTextInput();
	}

	close();
	return 0;
}