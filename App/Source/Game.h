#include <Snake.h>
#include <vector>
#include <Fruit.h>

#pragma once

class Game {
public:
	Game();
	~Game();

	void Update(float dt);
	void Draw();
private:
	Snake m_Snake;
	std::vector<Fruit> m_Fruits;
};
