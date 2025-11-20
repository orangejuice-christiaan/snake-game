#pragma once

#include <vector>
#include "raylib.h"
#include "Fruit.h"
#include "Snake.h"

class Game {
public:
	Game();
	~Game();

	void Update(float dt);
	void Draw();
	void OnInput();

	const Camera3D& GetCamera() const;

private:
	void UpdateCameraTransform();
	Quaternion BuildOrientation() const;

	Camera3D m_Camera;
	Snake m_Snake;
	std::vector<Fruit> m_Fruits;
	float m_MoveSpeed;
	float m_MouseSensitivity;
	float m_Yaw;
	float m_Pitch;
	float m_EyeHeight;
	float m_CameraDistance;
	float m_MinCameraDistance;
	float m_MaxCameraDistance;
	float m_ZoomSpeed;
};
