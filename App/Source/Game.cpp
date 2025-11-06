#include <Game.h>
#include <Util/Logger.h>

Game::Game()
	: m_Snake(), m_Fruits()
{
	Transform& ts = m_Snake.m_Transform = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }
	};

	Logger::Log("Spawning Snake...");
}

Game::~Game()
{
}

void Game::Update(float dt)
{

}

void Game::Draw()
{
	Transform& ts = m_Snake.m_Transform;

	Vector3 start = { ts.translation.x, ts.translation.y, ts.translation.z };
	Vector3 end = { ts.translation.x, ts.translation.y + 2.0f, ts.translation.z };

	DrawCapsule(start, end, m_Snake.m_Radius, 64, 64, LIME);
}

void Game::OnInput()
{

}
