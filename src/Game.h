#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <entt/entt.hpp>

namespace match3
{

class Game
{
public:
	Game() = default;
	~Game();

	// Interface
	void init();
	void handleEvents();
	void update(double delta);
	void render();
	void clean();
	bool running();

private:
	// Constants
	const static int SCREEN_WIDTH = 640;
	const static int SCREEN_HEIGHT = 640;

	const static int LEVEL_SIZE = 10;
	const float SCALE = 2.f;

	// Members
	entt::registry registry;

	SDL_Window* window{ nullptr };
	SDL_Event gameEvent{ };
	SDL_Renderer* renderer{ nullptr };
	bool isRunning{ false };


	// todo: remove
	SDL_Texture* cellTexture;
	SDL_Texture* chipTexture;
	SDL_Rect* textureRect;
};

}