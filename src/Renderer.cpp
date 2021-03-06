#include "Renderer.hpp"
#include <math.h>

constexpr int txtH = 16;
constexpr int txtW = 16;

inline int min(int x, int y)
{
	return x < y ? x : y;
}


inline int max(int x, int y)
{
	return x > y ? x : y;
}

inline float square(int x)
{
	return x*x;
}

inline float absolute(int x)
{
	return x < 0 ? -x : x;
}

inline float textureMap(float a, float z0, float z1, float u0, float u1)
{
	return ((1 - a) * (u0/z0) + a*(u1/z1))
		/ (((1-a)/z0) + (a/z1));
}

Renderer::Renderer(SDL_Renderer* renderer)
	:m_render(renderer)
{
	SDL_Surface* s = SDL_LoadBMP("wall.bmp");
	m_txtWall = SDL_CreateTextureFromSurface(m_render, s);
	SDL_FreeSurface(s);

	s = SDL_LoadBMP("floor.bmp");
	m_txtFloor= SDL_CreateTextureFromSurface(m_render, s);
	SDL_FreeSurface(s);
}

Renderer::~Renderer()
{
	SDL_DestroyTexture(m_txtWall);
	SDL_DestroyTexture(m_txtFloor);
}

void Renderer::setResolution(int w, int h)
{
	m_w = w;
	m_h = h;
}

void Renderer::setCamera(VectorF position, float direction, float height)
{
	m_camPos = position;
	m_camDir = direction;
	m_camH = height;
}

void Renderer::renderLevel(const Level& level)
{
	m_remainingLines = m_w;
	m_linesDrawn.resize(m_w);
	for (auto& line : m_linesDrawn)
		line = 0;

	iterateNode(level.getRoot());
}

void Renderer::iterateNode(const Node& n)
{
	VectorF b = n.line.w.start - n.line.w.end;
	b = b / b.length();

	float bx = b.x;
	b.x = -b.y;
	b.y = bx;

	//if cy is positive player is above the line of the current node and under it if not
	float cy = (m_camPos - n.line.w.start) * b;

	if (cy > 0)
	{
		if (n.right)
			iterateNode(*n.right);
		renderLine(n.line);
		if (n.left)
			iterateNode(*n.left);
	}
	else
	{
		if (n.left)
			iterateNode(*n.left);
		renderLine(n.line);
		if (n.right)
			iterateNode(*n.right);
	}
}

void Renderer::renderLine(const Line& l)
{
	VectorF ps, pe; //start and end point in camera perspective
	ps = toCamCoords(l.w.start);
	pe = toCamCoords(l.w.end);

	float length = (ps - pe).length();

	float txtS = 0; //Texture start coordinate (0 unless clipped wall)
	float txtE = length / 3; //Texture end coordinate (wall divided by wall height length unless clipped wall)

	//Clip if necesary with plane y = 0.01
	if (ps.y < 0.1 && pe.y < 0.1)
	{
		return; //The wall is behind the camera, it does not have to be drawn
	} //Clip with line y=0.01
	else {
		VectorF clipped;
		VectorF dir = ps - pe;
		clipped.y = 0.01;
		clipped.x = ps.x + (dir.x / dir.y) * (0.01 - ps.y);
		if (ps.y < 0.01)
		{
			txtS = (ps - clipped).length() / 3;
			ps = clipped;
		}
		else if (pe.y < 0.01)
		{
			txtE = (length - (pe - clipped).length()) / 3;
			pe = clipped;
		}
	}
	
	//The four corners in perspective
	VectorF pst, psd, pet, ped;
	perspective(ps, pst, psd, l.hfloor, l.hroof);
	perspective(pe, pet, ped, l.hfloor, l.hroof);


	//The four corners in screen coordinates
	VectorI sst, ssd, set, sed;
	sst = toScreenCoords(pst);
	ssd = toScreenCoords(psd);
	set = toScreenCoords(pet);
	sed = toScreenCoords(ped);

	int from = min(sst.x, set.x);
	int to = max(sst.x, set.x);
	int dx = to - from;
	int yfromTop = sst.x == from ? sst.y : set.y, ytoTop = sst.x == from ? set.y : sst.y;
	int yfromBot = sst.x == from ? ssd.y : sed.y, ytoBot = sst.x == from ? sed.y : ssd.y;

	float d1 = sst.x == from ? ps.y : pe.y; //Depth of the from line
	float d2 = sst.x == from ? pe.y : ps.y; //Depth of the to line
	float ydt = (float)(ytoTop - yfromTop) / dx;
	float ydb = (float)(ytoBot - yfromBot) / dx;

	SDL_Rect source = {0, 0, 1, txtH};
	SDL_Rect dest = {0, 0, 1, 0};

	int ct, cb;
	float tn; //Texture coord normalized
	float in; //i normalized
	for (int i = max(0, from); i < min(m_w - 1, to); i++)
	{
		if (m_linesDrawn[i])
			continue;
		
		in = (float)(i - from) / dx;
		ct = yfromTop + (i - from) * ydt;
		cb = yfromBot + (i - from) * ydb;

		if (from == sst.x)
		{
			tn = textureMap(in, d1, d2, txtS, txtE);
		}
		else
		{
			tn = textureMap(1 - in, d2, d1, txtS, txtE);
		}

		dest.x = i;
		dest.y = ct;
		dest.h = cb - ct;
		source.x = (int)(tn* txtW)%txtW;

		SDL_SetRenderDrawColor(m_render, l.roof.r, l.roof.g, l.roof.b, 0);
		SDL_RenderDrawLine(m_render, i, max(ct, 0), i, 0);

		if (l.w.texture)
		{
			SDL_RenderCopy(m_render, m_txtWall, &source, &dest);
			m_remainingLines--;
			m_linesDrawn[i] = true;
		}

		SDL_SetRenderDrawColor(m_render, l.floor.r, l.floor.g, l.floor.b, 0);
		SDL_RenderDrawLine(m_render, i, min(cb, m_h), i, m_h);
	}

}

VectorF Renderer::toCamCoords(const VectorF& point)
{
	return VectorF((point - m_camPos) * VectorF(m_camDir), (point - m_camPos) * VectorF(m_camDir + 90));
}

void Renderer::perspective(const VectorF& point, VectorF& top, VectorF& bottom, float floor, float roof)
{
	top = VectorF(point.x / point.y, (roof - m_camH) / point.y);
	bottom = VectorF(point.x / point.y, (floor -m_camH) / point.y);
}

VectorI Renderer::toScreenCoords(const VectorF& point)
{
	return VectorI(m_w + point.x * m_h, m_h - point.y * m_h) / 2;
}
