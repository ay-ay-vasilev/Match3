#pragma once
#include "Constants.h"

#include <SDL.h>
#include <SDL_image.h>
#include <entt/entt.hpp>

class System;

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

	// Members
	entt::registry registry;
	std::vector<System*> systems;
	constants::Constants constants;

	SDL_Window* window{ nullptr };
	SDL_Event gameEvent{ };
	SDL_Renderer* renderer{ nullptr };
	bool isRunning{ false };
};

}