#include <Game.h>
#include <Util/Logger.h>
#include <raymath.h>

Game::Game()
	: m_CameraProps {}
	, m_Snake()
	, m_Fruits()
	, m_MoveSpeed(3.0f)
	, m_FruitSpawnTimer(0.0f)
	, m_FruitSpawnInterval(1.0f)
	, m_FruitSpawnRadius(5.0f)
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
	m_CameraProps.maxDistance = 25.0f;
	m_CameraProps.zoomSpeed = 0.5f;

	m_CameraProps.camera = {};
	m_CameraProps.camera.up = { 0.0f, 1.0f, 0.0f };
	m_CameraProps.camera.fovy = 100.0f;
	m_CameraProps.camera.projection = CAMERA_PERSPECTIVE;
	m_totalDistance = 0.0f;
	m_lastHeadPos = m_Snake.m_Transform.translation;
	m_Snake.m_Segments.reserve(MAX_SEGMENTS);

	m_IsGameOver = false;
	m_GameOverTimer = 0.0f;

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
		ts.translation = Vector3Add(ts.translation, Vector3Scale(forwardFlat, Clamp(m_MoveSpeed + (m_Snake.m_Segments.size() * 0.1f), 1.0f, 10.0f) * dt));
	}

	UpdateCameraTransform();

	CheckFruitCollisions();

	RecordHeadPath();

	TryAddSnakeSegment();

	CheckSelfCollision();

	UpdateSnakeBody();

	float wheel = GetMouseWheelMove();
	if (wheel != 0.0f)
	{
		m_CameraProps.distance = Clamp(m_CameraProps.distance - wheel * m_CameraProps.zoomSpeed, m_CameraProps.minDistance, m_CameraProps.maxDistance);
	}

	m_FruitSpawnTimer += dt;
	if (m_FruitSpawnTimer >= m_FruitSpawnInterval && m_Fruits.size() < 100)
	{
		SpawnFruit();
		m_FruitSpawnTimer = 0.0f;
	}

	if (m_IsGameOver)
	{
		m_GameOverTimer += dt;
		if (m_GameOverTimer >= 5.0f)
		{
			m_IsGameOver = false;
			m_GameOverTimer = 0.0f;
		}
	}
}

void Game::Draw()
{
	const Transform& ts = m_Snake.m_Transform;

	Vector3 localStart = { 0.0f, 0.0f, -1.0f };
	Vector3 localEnd = { 0.0f, 0.0f, 1.0f };

	Vector3 start = Vector3Add(ts.translation, Vector3RotateByQuaternion(localStart, ts.rotation));
	Vector3 end = Vector3Add(ts.translation, Vector3RotateByQuaternion(localEnd, ts.rotation));

	DrawCapsule(start, end, m_Snake.m_Radius, 12, 4, m_Snake.m_Color);

	size_t segmentCount = m_Snake.m_Segments.size();
	for (size_t i = 0; i < segmentCount; i++)
	{
		const auto& seg = m_Snake.m_Segments[i];

		float t = static_cast<float>(i) / segmentCount;
		Color segColor {};
		segColor.r = static_cast<unsigned char>(m_Snake.m_Color.r * (1.0f - 0.5f * t));
		segColor.g = static_cast<unsigned char>(m_Snake.m_Color.g * (1.0f - 0.5f * t));
		segColor.b = static_cast<unsigned char>(m_Snake.m_Color.b * (1.0f - 0.5f * t));
		segColor.a = m_Snake.m_Color.a;

		Vector3 segStart = Vector3Add(seg.transform.translation, Vector3RotateByQuaternion({ 0.0f, 0.0f, -1.0f }, seg.transform.rotation));
		Vector3 segEnd = Vector3Add(seg.transform.translation, Vector3RotateByQuaternion({ 0.0f, 0.0f,  1.0f }, seg.transform.rotation));

		DrawCapsule(segStart, segEnd, m_Snake.m_Radius, 12, 4, segColor);
	}

	for (const Fruit& fruit : m_Fruits)
	{
		DrawCube(fruit.position, 0.5f, 0.5f, 0.5f, RED);
	}
}

void Game::DrawUI()
{
	auto speed = Clamp(m_MoveSpeed + (m_Snake.m_Segments.size() * 0.1f), 1.0f, 10.0f);
	char intStr[32];
	sprintf(intStr, "%.1f", speed);

	std::string speedStr = std::string("SPEED: ") + intStr;

	DrawText(speedStr.c_str(), 100, 10, 20, DARKGREEN);

	char lengthIntStr[32];
	sprintf(lengthIntStr, "%d", (int)m_Snake.m_Segments.size());
	std::string lengthStr = std::string("SNAKE_LENGTH: ") + lengthIntStr;

	DrawText(lengthStr.c_str(), GetScreenWidth() - 220, 10, 20, BLUE);

	if (m_IsGameOver)
	{
		const char* message = "U ded";
		int fontSize = 40;

		Vector2 textSize = MeasureTextEx(GetFontDefault(), message, fontSize, 1.0f);
		Vector2 pos = { GetScreenWidth() / 2 - textSize.x / 2, GetScreenHeight() / 2 - textSize.y / 2 };

		Color overlayColor = { 0, 0, 0, 50 };
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), overlayColor);

		DrawTextEx(GetFontDefault(), message, pos, fontSize, 1.0f, RED);
	}
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
	Vector3 playerPosition = m_CameraProps.camera.position;

	// Create bounding box for the player (centered at player position)
	const float playerBoxSize = 0.5f;
	const float playerHalfSize = playerBoxSize * 0.5f;
	BoundingBox playerBox {
		{
			playerPosition.x - playerHalfSize,
			playerPosition.y - playerHalfSize,
			playerPosition.z - playerHalfSize
		},
		{
			playerPosition.x + playerHalfSize,
			playerPosition.y + playerHalfSize,
			playerPosition.z + playerHalfSize
		}
	};

	// Check collision with each fruit
	const float fruitCubeSize = 0.5f;
	const float fruitHalfSize = fruitCubeSize * 0.5f;

	for (auto it = m_Fruits.begin(); it != m_Fruits.end();)
	{
		bool c = true;
		if (Vector3DistanceSqr(it->position, playerPosition) < 25)
		{
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

			if (CheckCollisionBoxes(playerBox, fruitBox))
			{
				m_Snake.m_DesiredSegmentCount++;
				it = m_Fruits.erase(it);
				c = false;
			}
		}
		if (c)
			++it;
	}
}

void Game::RecordHeadPath()
{
	const Transform& ts = m_Snake.m_Transform;

	float delta = Vector3Distance(ts.translation, m_lastHeadPos);

	if (delta < PATH_SAMPLE_DISTANCE)
		return;

	m_totalDistance += delta;

	m_PathBuffer.push({
		ts.translation,
		ts.rotation,
		m_totalDistance
		});

	m_lastHeadPos = ts.translation;
}

void Game::UpdateSnakeBody()
{
	int pathSize = (int)m_PathBuffer.getSize();
	if (pathSize < 2) return;

	for (int i = 0; i < m_Snake.m_Segments.size(); i++)
	{
		auto& seg = m_Snake.m_Segments[i];
		float targetDist = m_totalDistance - (i + 1) * SEGMENT_SPACING;

		// Initialize once
		if (seg.pathIndex < 0)
			seg.pathIndex = 0;

		// Move FORWARD along the path as needed
		while (seg.pathIndex + 1 < pathSize &&
			m_PathBuffer[seg.pathIndex + 1].distance < targetDist)
		{
			seg.pathIndex++;
		}

		// Ensure we have a valid segment
		if (seg.pathIndex + 1 >= pathSize)
			continue;

		const PathPoint& a = m_PathBuffer[seg.pathIndex];
		const PathPoint& b = m_PathBuffer[seg.pathIndex + 1];

		float denom = (b.distance - a.distance);
		if (denom <= 0.0001f)
			continue;

		float t = (targetDist - a.distance) / denom;

		seg.transform.translation = Vector3Lerp(a.position, b.position, t);
		seg.transform.rotation = QuaternionSlerp(a.rotation, b.rotation, t);
	}
}

bool Game::SamplePath(float targetDistance, PathPoint& a, PathPoint& b)
{
	int size = (int)m_PathBuffer.getSize();
	if (size < 2) return false;

	for (int i = size - 1; i > 0; --i)
	{
		if (m_PathBuffer[static_cast<size_t>(i) - 1].distance <= targetDistance &&
			m_PathBuffer[i].distance >= targetDistance)
		{
			a = m_PathBuffer[static_cast<size_t>(i) - 1];
			b = m_PathBuffer[i];
			return true;
		}
	}
	return false;
}

void Game::TryAddSnakeSegment()
{
	int currentCount = (int)m_Snake.m_Segments.size();

	if (currentCount >= m_Snake.m_DesiredSegmentCount)
		return;

	float requiredDistance = (currentCount + 1) * SEGMENT_SPACING;

	if (m_totalDistance >= requiredDistance)
		m_Snake.m_Segments.push_back({ Transform{}, -1 });;
}

void Game::CheckSelfCollision()
{
	const Transform& headTransform = m_Snake.m_Transform;
	Vector3 headPos = headTransform.translation;
	float headRadius = m_Snake.m_Radius;

	int safeSegments = 5;

	for (int i = safeSegments; i < m_Snake.m_Segments.size(); i++)
	{
		const SnakeSegment& seg = m_Snake.m_Segments[i];
		const Vector3 segPos = seg.transform.translation;
		float segRadius = m_Snake.m_Radius;

		if (CheckCollisionSpheres(headPos, headRadius, segPos, segRadius))
		{
			OnSelfCollision();
			break;
		}
	}
}

void Game::OnSelfCollision()
{
	m_IsGameOver = true;

	m_Snake.m_Segments.clear();
	m_Snake.m_DesiredSegmentCount = 0;

	m_totalDistance = 0.0f;
	m_PathBuffer.clear();
	m_lastHeadPos = m_Snake.m_Transform.translation;

	m_Fruits.clear();
}
