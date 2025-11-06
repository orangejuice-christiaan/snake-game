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

	Camera3D camera = { 0 };
	camera.position = { 5.0f, 5.0f, 5.0f };
	camera.target = { 0.0f, 1.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

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

		UpdateCamera(&camera, CAMERA_FIRST_PERSON);

		BeginDrawing();

		BeginMode3D(camera);

		ClearBackground(RAYWHITE);

		game.Draw();

		DrawGrid(100, 1.0f); // TEMP

		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();

		game.OnInput();
	}

	CloseWindow();

	return 0;
}