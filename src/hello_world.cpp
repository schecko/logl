
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "glad/glad.h"
#include <SDL2/SDL_opengl.h>

SDL_Surface* G_screenSurface = nullptr;
SDL_Window* G_screenWindow = nullptr;

SDL_Surface* loadImage(const char* path) {
	SDL_assert_paranoid(G_screenSurface);
	SDL_Surface* result = IMG_Load(path);
	if(result) {
		SDL_Surface* converted = SDL_ConvertSurface(result, G_screenSurface->format, 0);
		if(converted) {
			SDL_FreeSurface(result);
			result = converted;
		}
	}
	return result;
}

int main(int argc, char** argv) {

	if(SDL_Init(SDL_INIT_VIDEO)) {
		printf("sdl error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	int image_load = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	SDL_assert((image_load & IMG_INIT_JPG) && (image_load & IMG_INIT_PNG));


	G_screenWindow = SDL_CreateWindow(
		"Bleh",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		620, 387,
		SDL_WINDOW_OPENGL
	);
	if(!G_screenWindow) {
		printf("sdl create window error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	G_screenSurface = SDL_GetWindowSurface(G_screenWindow);

	SDL_Surface* randomImage = loadImage("./data/face.png");

	SDL_Rect blitDestination = {};
	blitDestination.w = G_screenSurface->w;
	blitDestination.h = G_screenSurface->h;
	SDL_BlitScaled(randomImage, 0, G_screenSurface, &blitDestination);

	{ // gl init
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GLContext gContext = SDL_GL_CreateContext(G_screenWindow);
		SDL_assert_release(gContext);

		gladLoadGLLoader(SDL_GL_GetProcAddress);
		// enable vsync
		SDL_GL_SetSwapInterval(1);

	}


	bool quit = false;
	SDL_Event e;
	while(!quit) {

		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) quit = true;
			switch(e.key.keysym.sym) {
				case SDLK_UP:
					break;
				case SDLK_DOWN:
					break;
				case SDLK_LEFT:
					break;
				case SDLK_RIGHT:
					break;
				default:
					break;
			}

		}

		//SDL_FillRect(G_screenSurface, NULL, SDL_MapRGB(G_screenSurface->format, r, g, b));
		SDL_UpdateWindowSurface(G_screenWindow);
	}

	SDL_DestroyWindow(G_screenWindow);

	IMG_Quit();
	SDL_Quit();

	printf("success");
	return 0;
}
