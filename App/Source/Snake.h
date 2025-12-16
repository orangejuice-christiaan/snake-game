#pragma once
#include <raylib.h>
#include <vector>

class Game;

struct SnakeSegment
{
	Transform transform;
	int pathIndex = -1;
};

class Snake
{
public:
	Snake();
	~Snake();

private:
	Transform m_Transform;
	float m_Radius;
	Color m_Color;
	std::vector<SnakeSegment> m_Segments;
	int m_DesiredSegmentCount;

	friend class Game;
};

struct PathPoint
{
	Vector3 position;
	Quaternion rotation;
	float distance;
};

