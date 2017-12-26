#include <iostream>

#include <SDL2/SDL.h>

#include "Vector.hpp"
#include "Renderer.hpp"

constexpr Uint32 turnLeft = SDL_SCANCODE_LEFT;
constexpr Uint32 turnRight = SDL_SCANCODE_RIGHT;
constexpr Uint32 goForward = SDL_SCANCODE_UP;
constexpr Uint32 goBack = SDL_SCANCODE_DOWN;
constexpr Uint32 goUp = SDL_SCANCODE_SPACE;
constexpr Uint32 goDown = SDL_SCANCODE_LSHIFT;


void pollEvents();

SDL_Window* win = 0;
SDL_Renderer* render = 0;
bool running = true;

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	int err = SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_SHOWN, &win, &render);
	if (err < 0)
	{
		SDL_Quit();
		return -1;
	}

	Uint32 ms;
	Uint32 delta;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	VectorF camPos(0, 0);
	float camDir = 0;
	float camH = 1.5;
	Renderer renderer(render);
	renderer.setResolution(640, 480);

	Sector s1 = {
		{
			{VectorF(-10, 10), VectorF(10, 10), 0},
			{VectorF(10, 10), VectorF(10, -20), 0},
			{VectorF(10, -20), VectorF(-5, -20), 0},
			{VectorF(-5, -20), VectorF(-5, 0), 0},
			{VectorF(-5, 0), VectorF(-10, 0), 0},
			{VectorF(-10, 0), VectorF(-10, 10), 0}
		},
		{50, 50, 255},
		{100, 100, 100},
		3,
		0
	};

	/*Sector s1 = {
		{
			{VectorF(-10, 10), VectorF(10, 10), 0},
			{VectorF(10, 10), VectorF(10, -10), 0},
			{VectorF(10, -10), VectorF(-10, -10), 0},
			{VectorF(-10, -10), VectorF(-10, 10), 0}
		},
		{50, 50, 255},
		{100, 100, 100},
		3,
		0
	};*/

	Level level({s1});

	while(running)
	{
		ms = SDL_GetTicks();
		
		pollEvents();

		if (keys[turnRight])
		{
			camDir -= 2;
			if (camDir < 0) camDir += 360;
		}
		if (keys[turnLeft])
		{
			camDir += 2;
			if (camDir > 360) camDir -= 360;
		}
		if (keys[goForward])
		{
			camPos = camPos + VectorF(camDir+90) * 0.1;
		}
		if (keys[goBack])
		{
			camPos = camPos - VectorF(camDir+90) * 0.1;
		}
		if (keys[goUp])
		{
			camH += 0.05;
			if (camH > 2.5) camH = 2.5;
		}
		if (keys[goDown])
		{
			camH -= 0.05;
			if (camH < 0.5) camH = 0.5;
		}

		renderer.setCamera(camPos, camDir, camH);

		renderer.renderLevel(level);

		SDL_RenderPresent(render);

		delta = SDL_GetTicks() - ms;

		if (delta < 1000 / 60)
		{
			SDL_Delay((1000/60) - delta);
		}

	}


	SDL_Quit();
	return 0;
}

void pollEvents()
{
	static SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		}
	}
}

