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

				glEnable(GL_DEPTH_TEST);

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
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
		const char *glsl_version = "#version 460";
		ImGui_ImplOpenGL3_Init(glsl_version);

		SDL_Event event;
		SDL_StartTextInput();

    	auto start = std::chrono::steady_clock::now();
		int fps = 0;
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

			ImGui::Begin("Controls");

			ImGui::SliderInt("simulation_steps",&game_state.m_simulation_steps,1,10000);

			ImGui::SliderFloat3("sphere_pos", &game_state.m_sphere_pos[0], -1.0f, 1.0f);
			ImGui::SliderFloat("sphere_rad", &game_state.m_sphere_rad, 0.01f, 1.0f);
			if (ImGui::Button("on/ff"))
			{
				game_state.m_sphere_active = !game_state.m_sphere_active;
			}

			ImGui::End();
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			SDL_GL_SwapWindow(gWindow);

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;

			if(elapsed_seconds.count() > 1.0){
				std::cout << "fps: " << fps << std::endl;
				start = std::chrono::steady_clock::now();
				fps = 0;
			}else{
				fps += 1;
			}
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