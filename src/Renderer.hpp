#pragma once
#include <SDL2/SDL.h>

#include "Vector.hpp"

struct Wall {
	VectorF start, end;
	Uint8 r, g, b;
};

class Renderer {
public:
	Renderer(SDL_Renderer* render);
	virtual ~Renderer();
	void setResolution(int w, int h);

	void setCamera(VectorF position, float direction, float height);
	void renderWall(const Wall& wall);
private:
	VectorF toCamCoords(const VectorF& point);
	void perspective(const VectorF& point, VectorF& top, VectorF& bottom);
	VectorI toScreenCoords(const VectorF& point);

	VectorF m_camPos;
	float m_camDir, m_camH;
	SDL_Renderer* m_render;
	SDL_Texture* m_txtWall, *m_txtFloor;

	int m_w, m_h;
};

