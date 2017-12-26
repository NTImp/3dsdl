#include "Level.hpp"

#include <iostream>

/*void printLevel(const Node& root, int identation)
{
	for (int i=0; i < identation; i++)
		std::cout << " ";
	std::cout	<< identation << ": " << root.line.w.start.x << "x" << root.line.w.start.y
			<< " -> " << root.line.w.end.x << "x" << root.line.w.end.y << std::endl;
	if (root.right)
		printLevel(*root.right, identation + 1);
	if (root.left)
		printLevel(*root.left, identation + 1);
}*/

Level::Level(const std::vector<Sector>& raw)
{
	std::vector<Line> lines;
	for (auto& sector : raw)
	{
		for (auto& wall : sector.walls)
		{
			Line l;
			l.w = wall;
			l.floor = sector.floor;
			l.roof = sector.roof;
			l.hroof = sector.hroof;
			l.hfloor = sector.hfloor;

			lines.push_back(l);
		}
	}

	makeNode(tree, lines);
}

void Level::splitLine(const Line& split, const Line& line, Line& l1, Line& l2)
{
	VectorF p1 = line.w.start;
	VectorF p2 = line.w.end;
	VectorF p3 = split.w.start;
	VectorF p4 = split.w.end;

	//Parameters for the implicit form of th two lines (y = a + bx)
	float t1 = p1.x * p2.y - p1.y * p2.x;
	float t2 = p3.x * p4.y - p3.y * p4.x;
	float bt = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	float ix = ((p3.x - p4.x) * t1 - (p1.x - p2.x) * t2) / bt;
	float iy = ((p3.y - p4.y) * t1 - (p1.y - p2.y) * t2) / bt;

	VectorF intersect(ix, iy);

	l1 = line; l2 = line;

	l1.w.end = intersect;
	l2.w.start = intersect;
}

void Level::makeNode(Node& node, const std::vector<Line>& lines)
{
	std::vector<Line> l, r;
	node.line = lines[lines.size() / 2];

	//b1 and b2 make an orthonormal base with b1 inside the split line
	VectorF b1 = node.line.w.start - node.line.w.end;
	b1 = b1 / b1.length(); //Normalize b1

	VectorF b2 = VectorF(-b1.y, b1.x); //create a perpendicular vector to b1

	VectorF p = node.line.w.start; //A point from the split rect
	for (int i = 0; i < lines.size(); i++)
	{
		auto& line = lines[i];

		//ignore if is the split line
		if (i == lines.size() / 2)
			continue;

		//The y coordinates of the two extremes of the current line in the base <b1, b2>
		float 	y1 = (line.w.start - p) * b2,
			y2 = (line.w.end - p) * b2;

		if (y1 > 0.01)
		{
			if (y2 > 0.01)
			{
				r.push_back(line);
			}
			else if (y2 < -0.01)
			{
				Line l1, l2;
				splitLine(node.line, line, l1, l2);
				r.push_back(l1);
				l.push_back(l2);
			}
			else
			{
				r.push_back(line);
			}
		}
		else if (y1 < -0.01)
		{
			if (y2 < -0.01)
			{
				l.push_back(line);
			}
			else if (y2 > 0.01)
			{
				Line l1, l2;
				splitLine(node.line, line, l1, l2);
				l.push_back(l1);
				r.push_back(l2);
			}
			else
			{
				l.push_back(line);
			}
		}
		else
		{
			if (y2 < -0.01)
			{
				l.push_back(line);
			}
			else if (y2 > 0.01)
			{
				r.push_back(line);
			}
			else
			{
				//The line is a single point and can be ignored
			}
		}
	}

	if (r.size() > 0)
	{
		node.right = std::make_unique<Node>();
		makeNode(*node.right, r);
	}
	if (l.size() > 0)
	{
		node.left = std::make_unique<Node>();
		makeNode(*node.left, l);
	}
}
