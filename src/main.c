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
#define NB_BASE_SHAPE 7
#define NB_SHAPES 1000
#define SHAPE_WIDTH 40
#define GAME_WIDTH 10
#define GAME_HEIGHT HEIGHT/SHAPE_WIDTH

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

typedef struct shape {
	SDL_Rect pos;
	int matrix[4][4];
} shape;

void blitShape(SDL_Surface* screen, shape* shape, SDL_Surface* sprite, SDL_Rect origin)
{
	int x= shape->pos.x*40 + origin.x;
	int y= shape->pos.y*40 + origin.y;
	
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if(shape->matrix[i][j])
			{
				SDL_Rect position = {
					x+40*i,
					y+40*j};
				SDL_BlitSurface(sprite, NULL, screen, &position);
			}
		}
	}
}

void blitScreen(SDL_Surface* screen, int g[GAME_WIDTH][GAME_HEIGHT], SDL_Surface* sprite, SDL_Rect origin)
{
	for(int i=0; i<GAME_WIDTH; i++)
	{
		for(int j=0; j<GAME_HEIGHT; j++)
		{
			if(g[i][j]==1)
			{
				SDL_Rect position = {
					origin.x + i*40,
					origin.y + j*40};
				SDL_BlitSurface(sprite, NULL, screen, &position);
			}
		}
	}
}

void initShape(shape* shape)
{	
	int baseShape[7][4][4]= {{
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 1, 1, 1, 1}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 1},
		{ 0, 1, 1, 1}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 1, 1},
		{ 0, 0, 1, 1}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 1, 0, 1}},
	    {{ 0, 0, 0, 0},
		{ 0, 1, 0, 0},
		{ 1, 1, 0, 0},
		{ 0, 1, 0, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 1, 1, 0, 0},
		{ 0, 1, 1, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 0, 0},
		{ 0, 1, 1, 0},
		{ 1, 1, 0, 0}}};
	int select = rand()%7;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			shape->matrix[i][j] = baseShape[select][i][j];
		}
	}

	shape->pos.x=0;
	shape->pos.y=0;
}

int collision(shape* s, int g[GAME_WIDTH][GAME_HEIGHT])
{
	if(s->pos.y+4>=GAME_HEIGHT)
		return 1;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if ((s->matrix[i][j]==1) && (g[s->pos.x+i][s->pos.y+j+1]==1))
			{
				return 1;
			}
		}
	}
	return 0;
}

void update(int g[GAME_WIDTH][GAME_HEIGHT], shape* s)
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if(s->matrix[i][j])
			{
				g[s->pos.x+i][s->pos.y+j]=1;
			}
		}
	}
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
	SDL_Surface *sprite = IMG_Load("../ressources/shape.png");
	SDL_Rect origin = {.x=200, .y=0};

	shape shape;
	initShape(&shape);

	int  game[GAME_WIDTH][GAME_HEIGHT];

	for(int i=0; i<GAME_WIDTH; i++)
	{
		for(int j=0; j<GAME_HEIGHT; j++)
		{
			game[i][j] = 0;
		}
	}

	srand(time(NULL));

	//Main loop
	int running=1;
	int cmpt=0;
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
				case SDL_KEYDOWN:
					break;
				default:
					break;
			}
		}

		//Game stuff
		//move shape down
		if(cmpt>10)
		{
			shape.pos.y++;
			cmpt=0;
		}
		else
		{
			cmpt++;
		}

		if(collision(&shape, game))
		{
			update(game, &shape);
			initShape(&shape);
		}


		//Print stuff
		//Starts with background
		SDL_BlitSurface(background, NULL, screen, NULL);
		blitShape(screen, &shape, sprite, origin);
		blitScreen(screen, game, sprite, origin);

		SDL_Flip(screen);
		tick();
	}

	return EXIT_SUCCESS;
}
