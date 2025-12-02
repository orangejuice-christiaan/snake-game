#pragma once
#include <raylib.h>

class Game;

class Snake {
public:
	Snake();
	~Snake();

private:
	Transform m_Transform;
	float m_Radius;
	Color m_Color;
	int m_Length;

	friend class Game;
};