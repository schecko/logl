
#include <stdio.h>
#include <SDL2/SDL.h>


SDL_Surface* G_screenSurface = nullptr;
SDL_Window* G_screenWindow = nullptr;

SDL_Surface* loadImage(char* path) {
	SDL_Surface* result = SDL_LoadBMP(path);
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

	SDL_Surface* randomImage = loadImage("data/face.png");

	SDL_Rect blitDestination = {};
	blitDestination.w = 100;
	blitDestination.h = 100;
	SDL_BlitScaled(randomImage, 0, G_screenSurface, &blitDestination);

	SDL_Delay(2000);

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

		//SDL_FillRect(G_screenSurface, NULL, SDL_MapRGB(G_screenSurface->format, r, g, b));
		SDL_UpdateWindowSurface(G_screenWindow);
	}

	SDL_DestroyWindow(G_screenWindow);

	SDL_Quit();

	printf("success");
	return 0;
}
