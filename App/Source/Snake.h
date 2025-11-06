#pragma once
#include <raylib.h>

class Game;

class Snake {
public:
	Snake();
	~Snake();

	void Move();

private:
	Transform m_Transform;
	float m_Radius;
	Color m_Color;

	friend class Game;
};