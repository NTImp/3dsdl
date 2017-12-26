#pragma once
#include <memory>

#include "LevelData.hpp"

//Wall stored in the level
struct Line {
	Wall w;
	Color floor, roof;
	float hroof, hfloor;
};

struct Node {
	Line line;
	std::unique_ptr<Node> left, right;
};

class Level {
public:
	Level(const std::vector<Sector>& raw);

	const Node& getRoot() const { return tree; };
private:
	void makeNode(Node& node, const std::vector<Line>& lines);
	void splitLine(const Line& split, const Line& line, Line& b, Line& t);
	Node tree;
};
