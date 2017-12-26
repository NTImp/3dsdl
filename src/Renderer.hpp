#pragma once
#include <SDL2/SDL.h>

#include "Level.hpp"

class Renderer {
public:
	Renderer(SDL_Renderer* render);
	virtual ~Renderer();
	void setResolution(int w, int h);

	void setCamera(VectorF position, float direction, float height);

	void renderLevel(const Level& lvl);
private:
	void iterateNode(const Node& node);
	void renderLine(const Line& line);

	VectorF toCamCoords(const VectorF& point);
	void perspective(const VectorF& point, VectorF& top, VectorF& bottom, float floor, float roof);
	VectorI toScreenCoords(const VectorF& point);

	VectorF m_camPos;
	float m_camDir, m_camH;
	SDL_Renderer* m_render;
	SDL_Texture* m_txtWall, *m_txtFloor;

	std::vector<char> m_linesDrawn;
	int m_remainingLines;

	int m_w, m_h;
};

