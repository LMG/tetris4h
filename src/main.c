#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char* argv[])
{
	SDL_Surface *screen = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		perror("Error initiating SDL.");
		return EXIT_FAILURE;
	}

	if((screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_HWSURFACE)) == NULL)
	{
		perror("Error setting video mode.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
