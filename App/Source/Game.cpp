#include <Game.h>
#include <Util/Logger.h>
#include <raymath.h>

Game::Game()
	: m_CameraProps{}
	, m_Snake()
	, m_Fruits()
	, m_MoveSpeed(3.0f)
	, m_FruitSpawnTimer(0.0f)
	, m_FruitSpawnInterval(2.0f)
	, m_FruitSpawnRadius(20.0f)
	, m_FruitSpawnMinHeight(-5.0f)
	, m_FruitSpawnMaxHeight(10.0f)
{
	HideCursor();
	DisableCursor();

	Transform& ts = m_Snake.m_Transform = {
		{ 0.0f, 0.5f, 0.0f },
		QuaternionIdentity(),
		{ 1.0f, 1.0f, 1.0f }
	};

	m_CameraProps.mouseSensitivity = 0.002f;
	m_CameraProps.yaw = 0.0f;
	m_CameraProps.pitch = 0.0f;
	m_CameraProps.eyeHeight = 0.5f;
	m_CameraProps.distance = 0.0f;
	m_CameraProps.minDistance = 0.0f;
	m_CameraProps.maxDistance = 6.0f;
	m_CameraProps.zoomSpeed = 0.5f;

	m_CameraProps.camera = {};
	m_CameraProps.camera.up = { 0.0f, 1.0f, 0.0f };
	m_CameraProps.camera.fovy = 100.0f;
	m_CameraProps.camera.projection = CAMERA_PERSPECTIVE;

	UpdateCameraTransform();

	Logger::Log("Spawning Snake...");
}

Game::~Game()
{
}

void Game::Update(float dt)
{
	Vector2 mouseDelta = GetMouseDelta();
	m_CameraProps.yaw += -mouseDelta.x * m_CameraProps.mouseSensitivity;
	m_CameraProps.pitch += mouseDelta.y * m_CameraProps.mouseSensitivity;

	const float pitchLimit = DEG2RAD * 89.0f;
	m_CameraProps.pitch = Clamp(m_CameraProps.pitch, -pitchLimit, pitchLimit);

	Transform& ts = m_Snake.m_Transform;

	const Quaternion orientation = BuildOrientation();
	ts.rotation = orientation;

	Vector3 forward = Vector3RotateByQuaternion({ 0.0f, 0.0f, 1.0f }, orientation);

	Vector3 forwardFlat = { forward.x, forward.y, forward.z };

	if (Vector3LengthSqr(forwardFlat) > 0.0f)
	{
		forwardFlat = Vector3Normalize(forwardFlat);
		ts.translation = Vector3Add(ts.translation, Vector3Scale(forwardFlat, Clamp(m_MoveSpeed + (m_Snake.m_Length * 0.1f), 1.0f, 10.0f) * dt));
	}

	UpdateCameraTransform();

	// Check for fruit collisions
	CheckFruitCollisions();

	float wheel = GetMouseWheelMove();
	if (wheel != 0.0f)
	{
		m_CameraProps.distance = Clamp(m_CameraProps.distance - wheel * m_CameraProps.zoomSpeed, m_CameraProps.minDistance, m_CameraProps.maxDistance);
	}

	// Update fruit spawn timer
	m_FruitSpawnTimer += dt;
	if (m_FruitSpawnTimer >= m_FruitSpawnInterval && m_Fruits.size() < 25)
	{
		SpawnFruit();
		m_FruitSpawnTimer = 0.0f;
	}
}

void Game::Draw()
{
	const Transform& ts = m_Snake.m_Transform;

	// Draw snake head
	Vector3 localStart = { 0.0f, 0.0f, -1.0f };
	Vector3 localEnd = { 0.0f, 0.0f, 1.0f };

	Vector3 start = Vector3Add(ts.translation, Vector3RotateByQuaternion(localStart, ts.rotation));
	Vector3 end = Vector3Add(ts.translation, Vector3RotateByQuaternion(localEnd, ts.rotation));

	DrawCapsule(start, end, m_Snake.m_Radius, 64, 64, m_Snake.m_Color);

	// Draw fruits as red cubes
	for (const Fruit& fruit : m_Fruits)
	{
		DrawCube(fruit.position, 0.5f, 0.5f, 0.5f, RED);
	}

}

void Game::DrawUI()
{
	auto speed = Clamp(m_MoveSpeed + (m_Snake.m_Length * 0.1f), 1.0f, 10.0f);
	char intStr[32];
	sprintf(intStr, "%f", speed);
	DrawText(intStr, 100, 10, 20, DARKGREEN);
}

void Game::OnInput()
{
	if (IsKeyPressed(KEY_H))
	{
		if (IsCursorHidden())
		{
			ShowCursor();
			EnableCursor();
		}
		else
		{
			HideCursor();
			DisableCursor();
		}
	}
}

const Camera3D& Game::GetCamera() const
{
	return m_CameraProps.camera;
}

void Game::UpdateCameraTransform()
{
	const Transform& ts = m_Snake.m_Transform;

	Vector3 cameraPosition = {
		ts.translation.x,
		ts.translation.y + m_CameraProps.eyeHeight,
		ts.translation.z
	};

	const Quaternion orientation = BuildOrientation();
	Vector3 forward = Vector3RotateByQuaternion({ 0.0f, 0.0f, 1.0f }, orientation);
	Vector3 up = Vector3RotateByQuaternion({ 0.0f, 1.0f, 0.0f }, orientation);
	up = Vector3Normalize(up);

	Vector3 target = {
		cameraPosition.x + forward.x,
		cameraPosition.y + forward.y,
		cameraPosition.z + forward.z
	};

	if (m_CameraProps.distance > 0.0f)
	{
		Vector3 offset = Vector3Scale(forward, m_CameraProps.distance);
		cameraPosition = Vector3Subtract(cameraPosition, offset);
	}

	m_CameraProps.camera.position = cameraPosition;
	m_CameraProps.camera.target = target;
	m_CameraProps.camera.up = up;
}

Quaternion Game::BuildOrientation() const
{
	return QuaternionFromEuler(m_CameraProps.pitch, m_CameraProps.yaw, 0.0f);
}

void Game::SpawnFruit()
{
	const Transform& ts = m_Snake.m_Transform;
	Vector3 snakeHeadPos = ts.translation;

	// Generate random angle and distance within radius
	float angle = GetRandomValue(0, 360) * DEG2RAD;
	float distance = GetRandomValue(0, static_cast<int>(m_FruitSpawnRadius * 100)) / 100.0f;

	// Generate random Y position within height range
	float randomY = GetRandomValue(
		static_cast<int>(m_FruitSpawnMinHeight * 100),
		static_cast<int>(m_FruitSpawnMaxHeight * 100)
	) / 100.0f;

	// Calculate position in 3D space around the snake head
	Vector3 offset = {
		cosf(angle) * distance,
		randomY,
		sinf(angle) * distance
	};

	Fruit fruit;
	fruit.position = Vector3Add(snakeHeadPos, offset);
	fruit.type = APPLE;

	m_Fruits.push_back(fruit);
}

void Game::CheckFruitCollisions()
{
	// Use player/snake position as the center of the player's collision box
	const Transform& ts = m_Snake.m_Transform;
	Vector3 playerPosition = ts.translation;

	// Create bounding box for the player (centered at player position)
	const float playerBoxSize = 0.5f;
	const float playerHalfSize = playerBoxSize * 0.5f;
	BoundingBox playerBox;
	playerBox.min = {
		playerPosition.x - playerHalfSize,
		playerPosition.y - playerHalfSize,
		playerPosition.z - playerHalfSize
	};
	playerBox.max = {
		playerPosition.x + playerHalfSize,
		playerPosition.y + playerHalfSize,
		playerPosition.z + playerHalfSize
	};

	// Check collision with each fruit
	const float fruitCubeSize = 0.5f;
	const float fruitHalfSize = fruitCubeSize * 0.5f;

	for (auto it = m_Fruits.begin(); it != m_Fruits.end();)
	{
		// Create bounding box for the fruit cube
		BoundingBox fruitBox;
		fruitBox.min = {
			it->position.x - fruitHalfSize,
			it->position.y - fruitHalfSize,
			it->position.z - fruitHalfSize
		};
		fruitBox.max = {
			it->position.x + fruitHalfSize,
			it->position.y + fruitHalfSize,
			it->position.z + fruitHalfSize
		};

		// Check box-box collision
		if (CheckCollisionBoxes(playerBox, fruitBox))
		{
			// Collision detected - increase snake length and remove fruit
			m_Snake.m_Length++;
			it = m_Fruits.erase(it);
		}
		else
		{
			++it;
		}
	}
}
