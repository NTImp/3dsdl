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


void Renderer::renderWall(const Wall& w)
{
	VectorF ps, pe; //start and end point in camera perspective
	ps = toCamCoords(w.start);
	pe = toCamCoords(w.end);

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
	perspective(ps, pst, psd);
	perspective(pe, pet, ped);


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
	for (int i = max(0, from); i < min(m_w, to); i++)
	{
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

		SDL_SetRenderDrawColor(m_render, 0, 0, 0xff, 0);
		SDL_RenderDrawLine(m_render, i, max(ct, 0), i, 0);

		SDL_RenderCopy(m_render, m_txtWall, &source, &dest);

		SDL_SetRenderDrawColor(m_render, 0x55, 0x55, 0x55, 0);
		SDL_RenderDrawLine(m_render, i, min(cb, m_h), i, m_h);
	}

}

VectorF Renderer::toCamCoords(const VectorF& point)
{
	return VectorF((point - m_camPos) * VectorF(m_camDir), (point - m_camPos) * VectorF(m_camDir + 90));
}

void Renderer::perspective(const VectorF& point, VectorF& top, VectorF& bottom)
{
	top = VectorF(point.x / point.y, (3 - m_camH) / point.y);
	bottom = VectorF(point.x / point.y, (-m_camH) / point.y);
}

VectorI Renderer::toScreenCoords(const VectorF& point)
{
	return VectorI(m_w + point.x * m_h, m_h - point.y * m_h) / 2;
}
