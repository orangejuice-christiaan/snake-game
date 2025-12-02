#include "raylib.h"
#include "Game.h"

int main(void)
{
	const int screenWidth = 1200;
	const int screenHeight = 800;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetConfigFlags(FLAG_VSYNC_HINT);

	InitWindow(screenWidth, screenHeight, "Snake Game");

	SetTargetFPS(144);

	float deltaTime = 1.0f / 60.0f;
	double accumulator = 0.0f;
	double lastTime = GetTime();

	Game game;

	while (!WindowShouldClose())
	{
		double currentTime = GetTime();
		double frameTime = currentTime - lastTime;
		lastTime = currentTime;
		accumulator += frameTime;

		while (accumulator >= deltaTime)
		{
			game.Update(deltaTime);
			accumulator -= deltaTime;
		}

		BeginDrawing();

		const Camera3D& camera = game.GetCamera();

		BeginMode3D(camera);

		ClearBackground(RAYWHITE);

		game.Draw();

		DrawGrid(100, 1.0f); // TEMP

		EndMode3D();

		DrawFPS(10, 10);

		game.DrawUI();

		EndDrawing();

		game.OnInput();
	}

	CloseWindow();

	return 0;
}