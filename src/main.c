#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
//tick in us
#define TICK 50000
#define NB_BASE_SHAPE 7
#define NB_SHAPES 1000
#define SHAPE_WIDTH 40
#define GAME_WIDTH 10
#define GAME_HEIGHT HEIGHT/SHAPE_WIDTH
#define ORIGIN_X 200
#define LOCK_TIME 30
#define FALL_SPEED 10

//allows to manage main loop speed
void tick()
{
	static clock_t previousTime = 0;
	
	clock_t runTime = clock()-previousTime;

	if(runTime > TICK)
	{
		printf("Can't keep up ! Did the system time change or is the computer overloaded? (runtime = %d, previoustime = %d, clock = %d)\n", (int) runTime, (int) previousTime, (int) clock());
	}
	else
	{
		usleep((TICK-runTime)/(CLOCKS_PER_SEC/1000000));
	}
	
	previousTime = clock();
}

typedef struct shape {
	SDL_Rect pos;
	int matrix[4][4];
} shape;

void blitShape(SDL_Surface* screen, shape* shape, SDL_Surface* sprite)
{
	int x= shape->pos.x*40 + ORIGIN_X;
	int y= shape->pos.y*40;
	
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

void blitScreen(SDL_Surface* screen, int g[GAME_WIDTH][GAME_HEIGHT], SDL_Surface* sprite)
{
	for(int i=0; i<GAME_WIDTH; i++)
	{
		for(int j=0; j<GAME_HEIGHT; j++)
		{
			if(g[i][j]==1)
			{
				SDL_Rect position = {
					ORIGIN_X + i*40,
					j*40};
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
		{ 1, 1, 1, 1},
		{ 0, 0, 0, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 0, 1},
		{ 0, 1, 1, 1},
		{ 0, 0, 0, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 1, 1, 0},
		{ 0, 1, 1, 0},
		{ 0, 0, 0, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 1, 1, 1},
		{ 0, 0, 0, 1},
		{ 0, 0, 0, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 1, 1},
		{ 0, 0, 0, 1},
		{ 0, 0, 0, 1}},
	    {{ 0, 0, 0, 0},
		{ 0, 1, 1, 0},
		{ 0, 0, 1, 1},
		{ 0, 0, 0, 0}},
	    {{ 0, 0, 0, 0},
		{ 0, 0, 1, 1},
		{ 0, 1, 1, 0},
		{ 0, 0, 0, 0}}};
	int select = rand()%7;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			shape->matrix[i][j] = baseShape[select][i][j];
		}
	}

	shape->pos.x=rand()%(GAME_WIDTH-4);
	shape->pos.y=-4;
}

int yCollision(shape* s, int g[GAME_WIDTH][GAME_HEIGHT])
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			int blockX = s->pos.x+i;
			int blockY = s->pos.y+j;
			int isBlock = s->matrix[i][j];
			if (isBlock && blockY >= 0
					&& (( blockY < GAME_HEIGHT 
						&& g[blockX][blockY+1])
						|| blockY+1 >= GAME_HEIGHT))
			{
				return 1;
			}
		}
	}
	return 0;
}

int xCollision(shape* s, int g[GAME_WIDTH][GAME_HEIGHT], int direction)
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if (direction >0)
			{
				if (s->matrix[i][j] && s->pos.x+i >= GAME_WIDTH)
				{
					return 1;
				}
				else if ((s->matrix[i][j]==1) && (g[s->pos.x+i+1][s->pos.y+j]==1))
				{
					return 1;
				}
			}
			else if (direction < 0)
			{
				if (s->matrix[i][j] && s->pos.x+i <= 0)
				{
					return 1;
				}
				else if ((s->matrix[i][j]==1) && (g[s->pos.x+i-1][s->pos.y+j]==1))
				{
					return 1;
				}
			}
		}
	}
	return 0;	
}

int update(int g[GAME_WIDTH][GAME_HEIGHT], shape* s)
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if(s->matrix[i][j] 
					&& s->pos.x+i < GAME_WIDTH 
					&& s->pos.x+i >= 0
					&& s->pos.y+j < GAME_HEIGHT
					&& s->pos.y+j >=0)
			{
				g[s->pos.x+i][s->pos.y+j]=1;
			}
			else if(s->matrix[i][j])
			{
				printf("%d, %d, %d, %d\n", s->pos.x+i, s->pos.y+j, i, j);
				return 1;
			}
		}
	}
	return 0;
}

// Moves all blocks from game one line down for line deletion
void moveBlocks(int g[GAME_WIDTH][GAME_HEIGHT], int jStart)
{
	for(int i=0; i<GAME_WIDTH; i++)
	{
		for(int j=jStart; j>0; j--)
		{
			g[i][j] = g[i][j-1];
		}
	}
}


int lineFull(int g[GAME_WIDTH][GAME_HEIGHT], int j)
{	
	for(int i=0; i<GAME_WIDTH; i++)
	{
		if(!g[i][j])
			return 0;
	}
	return 1;
}

int checkLines(int g[GAME_WIDTH][GAME_HEIGHT])
{
	int nbLines=0;
	for (int j=0; j<GAME_HEIGHT; j++)
	{
		if(lineFull(g, j))
		{
			nbLines++;
			moveBlocks(g, j);
		}
	}
	return nbLines;
}

int canTurnLeft (shape* s, int g[GAME_WIDTH][GAME_HEIGHT])
{
	int canTurn = 1;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if(s->matrix[i][j] 
				&& (g[s->pos.x+j][s->pos.y+(3-i)]
					|| s->pos.x+j >= GAME_WIDTH
					|| s->pos.x+j < 0
					|| s->pos.y+(3-i) >= GAME_HEIGHT
					|| s->pos.y+(3-i) < 0))
			{
				canTurn = 0;
			}
		}
	}
	return canTurn;
}

int canTurnRight (shape* s, int g[GAME_WIDTH][GAME_HEIGHT])
{
	int canTurn = 1;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if(s->matrix[i][j] 
				&& g[s->pos.x+(3-j)][s->pos.y+i])
			{
				canTurn = 0;
			}
		}
	}
	return canTurn;
}

void turnShapeRight (shape* s)
{
	int aux[4][4];
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			aux[i][j] = s->matrix[i][j];
		}
	}
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			s->matrix[i][j] = aux[j][3-i];
		}
	}
}

void turnShapeLeft (shape* s)
{
	int aux[4][4];
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			aux[i][j] = s->matrix[i][j];
		}
	}
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			s->matrix[i][j] = aux[3-j][i];
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

	srand(time(NULL));

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


	//Main loop
	int running=1;
	int cmpt=0;
	int bottomLock=0;
	int score=0;
	while(running)
	{
		SDL_Event event;
		printf("shape x %d, shape y %d\n", shape.pos.x, shape.pos.y);

		//Manage events
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT: 
					running = 0;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_RIGHT:
							if(!xCollision(&shape, game, 1))
								shape.pos.x += 1;
							break;
						case SDLK_LEFT:
							if(!xCollision(&shape, game, -1))
								shape.pos.x -= 1;
							break;
						case SDLK_SPACE:
							while(!yCollision(&shape, game))
								shape.pos.y++;
							bottomLock=LOCK_TIME;
							break;
						case SDLK_a:
						case SDLK_UP:
							if(canTurnLeft(&shape, game))
								turnShapeLeft(&shape);
							break;
						case SDLK_z:
							if(canTurnRight(&shape, game))
								turnShapeRight(&shape);
							break;
						case SDLK_DOWN:
							if(!yCollision(&shape, game))
								shape.pos.y++;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		//lines
		int nbLines = checkLines(game);
		
		if(nbLines)
			score += pow(2, nbLines*2);
		printf("%d\n", score);

		//Game stuff
		//move shape down
		if(yCollision(&shape, game))
		{
			if(bottomLock < LOCK_TIME)
			{
				bottomLock++;
			}
			else
			{
				bottomLock=0;
				running = !update(game, &shape);
				initShape(&shape);
			}
		}
		//Manage fall speed
		else if(cmpt>FALL_SPEED)
		{
			shape.pos.y++;
			bottomLock=0;
			cmpt=0;
		}
		cmpt++;

		//Print stuff
		//Starts with background
		SDL_BlitSurface(background, NULL, screen, NULL);
		blitShape(screen, &shape, sprite);
		blitScreen(screen, game, sprite);

		SDL_Flip(screen);
		tick();
	}
	
	int waiting = 1;
	while(waiting)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);
		switch(event.type)
		{
			case SDL_QUIT:
				waiting=0;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_SPACE)
					waiting=0;
				break;
			default:
				break;
		}
	}

	return EXIT_SUCCESS;
}
