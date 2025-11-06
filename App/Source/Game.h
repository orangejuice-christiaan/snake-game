#pragma once

#include <vector>
#include <Fruit.h>
#include "Game.h"
#include "Snake.h"

class Game {
public:
	Game();
	~Game();

	void Update(float dt);
	void Draw();
	void OnInput();
private:
	Snake m_Snake;
	std::vector<Fruit> m_Fruits;
};
