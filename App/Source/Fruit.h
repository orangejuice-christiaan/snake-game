#pragma once
#include <raylib.h>

typedef enum FruitType {
	APPLE
};

struct Fruit {
	Vector3 position;
	FruitType type;
};