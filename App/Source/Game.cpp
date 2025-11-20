#include <Game.h>
#include <Util/Logger.h>
#include <raymath.h>

Game::Game()
	: m_Camera{}
	, m_Snake()
	, m_Fruits()
	, m_MoveSpeed(5.0f)
	, m_MouseSensitivity(0.0025f)
	, m_Yaw(0.0f)
	, m_Pitch(0.0f)
	, m_EyeHeight(0.5f)
	, m_CameraDistance(0.0f)
	, m_MinCameraDistance(0.0f)
	, m_MaxCameraDistance(6.0f)
	, m_ZoomSpeed(0.5f)
{
	HideCursor();
	DisableCursor();

	Transform& ts = m_Snake.m_Transform = {
		{ 0.0f, 0.5f, 0.0f },
		QuaternionIdentity(),
		{ 1.0f, 1.0f, 1.0f }
	};

	m_Camera = {};
	m_Camera.up = { 0.0f, 1.0f, 0.0f };
	m_Camera.fovy = 100.0f;
	m_Camera.projection = CAMERA_PERSPECTIVE;

	UpdateCameraTransform();

	Logger::Log("Spawning Snake...");
}

Game::~Game()
{
}

void Game::Update(float dt)
{
	Vector2 mouseDelta = GetMouseDelta();
	m_Yaw += -mouseDelta.x * m_MouseSensitivity;
	m_Pitch += mouseDelta.y * m_MouseSensitivity;

	const float pitchLimit = DEG2RAD * 89.0f;
	m_Pitch = Clamp(m_Pitch, -pitchLimit, pitchLimit);

	Transform& ts = m_Snake.m_Transform;

	const Quaternion orientation = BuildOrientation();
	ts.rotation = orientation;

	Vector3 forward = Vector3RotateByQuaternion({ 0.0f, 0.0f, 1.0f }, orientation);

	Vector3 forwardFlat = { forward.x, forward.y, forward.z };

	if (Vector3LengthSqr(forwardFlat) > 0.0f)
	{
		forwardFlat = Vector3Normalize(forwardFlat);
		ts.translation = Vector3Add(ts.translation, Vector3Scale(forwardFlat, m_MoveSpeed * dt));
	}

	UpdateCameraTransform();

	float wheel = GetMouseWheelMove();
	if (wheel != 0.0f)
	{
		m_CameraDistance = Clamp(m_CameraDistance - wheel * m_ZoomSpeed, m_MinCameraDistance, m_MaxCameraDistance);
	}
}

void Game::Draw()
{
	const Transform& ts = m_Snake.m_Transform;

	Vector3 localStart = { 0.0f, 0.0f, -1.0f };
	Vector3 localEnd = { 0.0f, 0.0f, 1.0f };

	Vector3 start = Vector3Add(ts.translation, Vector3RotateByQuaternion(localStart, ts.rotation));
	Vector3 end = Vector3Add(ts.translation, Vector3RotateByQuaternion(localEnd, ts.rotation));

	DrawCapsule(start, end, m_Snake.m_Radius, 64, 64, m_Snake.m_Color);
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
	return m_Camera;
}

void Game::UpdateCameraTransform()
{
	const Transform& ts = m_Snake.m_Transform;

	Vector3 cameraPosition = {
		ts.translation.x,
		ts.translation.y + m_EyeHeight,
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

	if (m_CameraDistance > 0.0f)
	{
		Vector3 offset = Vector3Scale(forward, m_CameraDistance);
		cameraPosition = Vector3Subtract(cameraPosition, offset);
	}

	m_Camera.position = cameraPosition;
	m_Camera.target = target;
	m_Camera.up = up;
}

Quaternion Game::BuildOrientation() const
{
	return QuaternionFromEuler(m_Pitch, m_Yaw, 0.0f);
}
