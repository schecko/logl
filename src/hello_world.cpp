
#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char** argv) {
	if(SDL_Init(SDL_INIT_VIDEO)) {
		printf("sdl error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow(
		"Bleh",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		620, 387,
		SDL_WINDOW_OPENGL
	);
	if(!window) {
		printf("sdl create window error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface* surface = SDL_GetWindowSurface(window);

	bool quit = false;
	SDL_Event e;
	int time = 0;
	int r = 0, g = 0, b = 0;
	while(!quit) {
		time++;

		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) quit = true;
			switch(e.key.keysym.sym) {
				case SDLK_UP:
					r = 30;
					g = 0;
					b = 255;
					break;
				case SDLK_DOWN:
					r = 255;
					g = 0;
					b = 255;
					break;
				case SDLK_LEFT:
					r = 0;
					g = 255;
					b = 0;
					break;
				case SDLK_RIGHT:
					r = 255;
					g = 0;
					b = 0;
					break;
				default:
					r = g = b = time & 0xFF;
					break;
			}

		}

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	printf("success");
	return 0;
}
