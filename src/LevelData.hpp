#pragma once
#include "Vector.hpp"

#include <vector>

using byte = unsigned char;

struct Color {
	unsigned char r, g, b;
};

struct Wall {
	VectorF start, end;
	byte* texture;
};

struct Sector {
	std::vector<Wall> walls;
	Color roof, floor;
	float hroof, hfloor;
};
