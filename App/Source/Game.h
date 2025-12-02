#pragma once

#include <vector>
#include <deque>
#include "raylib.h"
#include "Fruit.h"
#include "Snake.h"
#include "Util/Random.h"

struct CameraProperties {
	Camera3D camera;
	float mouseSensitivity;
	float yaw;
	float pitch;
	float eyeHeight;
	float distance;
	float minDistance;
	float maxDistance;
	float zoomSpeed;
};

class Game {
public:
	Game();
	~Game();

	void Update(float dt);
	void Draw();
	void DrawUI();
	void OnInput();

	const Camera3D& GetCamera() const;

private:
	void UpdateCameraTransform();
	Quaternion BuildOrientation() const;
	void SpawnFruit();
	void CheckFruitCollisions();

	CameraProperties m_CameraProps;
	Snake m_Snake;
	std::vector<Fruit> m_Fruits;
	float m_MoveSpeed;
	float m_FruitSpawnTimer;
	float m_FruitSpawnInterval;
	float m_FruitSpawnRadius;
	float m_FruitSpawnMinHeight;
	float m_FruitSpawnMaxHeight;
};