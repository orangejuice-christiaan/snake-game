#pragma once

#include <vector>
#include <deque>
#include "raylib.h"
#include "Fruit.h"
#include "Snake.h"
#include "Util/Random.h"
#include "Util/CircularBuffer.h"

constexpr int MAX_SEGMENTS = 500;
constexpr float SEGMENT_SPACING = 0.6f;
constexpr float PATH_SAMPLE_DISTANCE = 0.1f;
constexpr int MAX_PATH_POINTS = 4096;

struct CameraProperties
{
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

class Game
{
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
	void RecordHeadPath();
	void UpdateSnakeBody();
	bool SamplePath(float targetDistance, PathPoint& a, PathPoint& b);
	void TryAddSnakeSegment();
	void CheckSelfCollision();
	void OnSelfCollision();

	CameraProperties m_CameraProps;
	Snake m_Snake;
	std::vector<Fruit> m_Fruits;
	float m_MoveSpeed;
	float m_FruitSpawnTimer;
	float m_FruitSpawnInterval;
	float m_FruitSpawnRadius;
	float m_FruitSpawnMinHeight;
	float m_FruitSpawnMaxHeight;
	float m_totalDistance;
	Vector3 m_lastHeadPos;
	CircularBuffer<PathPoint, MAX_PATH_POINTS> m_PathBuffer;

	bool m_IsGameOver;
	float m_GameOverTimer;
};