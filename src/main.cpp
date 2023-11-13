#include "Game.h"

#include <memory>
#include <chrono>

const double FRAME_TIME = 1.0 / 8.0; // Target frame time (60 FPS)

int main(int argc, char* argv[])
{
	auto game = std::make_unique<Game>();
	game->init();
	
	auto previousTime = std::chrono::steady_clock::now();
	double accumulator = 0.0;

	while (game->running())
	{
		auto currentTime = std::chrono::steady_clock::now();
		double deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
		previousTime = currentTime;

		accumulator += deltaTime;

		game->handleEvents();

		while (accumulator >= FRAME_TIME)
		{
			game->update(FRAME_TIME);
			accumulator -= FRAME_TIME;
		}

		game->render();
	}
	game->clean();

	return 0;
}