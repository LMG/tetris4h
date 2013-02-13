#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 800
#define HEIGHT 600
//tick in ms
#define TICK 100

//allows to manage main loop speed
void tick()
{
	static int previousTime = 0;
	
	int runTime = time(NULL)-previousTime;

	if(runTime > TICK)
	{
		printf("Can't keep up ! Did the system time change or is the computer overloaded?\n");
	}
	else
	{
		usleep(TICK-runTime);
	}
	
	previousTime = time(NULL);
}

int main(int argc, char* argv[])
{
	//Init
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

	SDL_Surface *background = IMG_Load("../ressources/background.png");
	
	//Main loop
	int running=1;
	while(running)
	{
		SDL_Event event;

		//Manage events
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT: 
					running = 0;
					break;
				default:
					break;
			}
		}

		//Print stuff
		//Starts with background
		SDL_BlitSurface(background, NULL, screen, NULL);
		//for(int i=0; i<NB_


		SDL_Flip(screen);
		tick();
	}

	return EXIT_SUCCESS;
}
