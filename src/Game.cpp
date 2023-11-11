#include "Game.h"

#include "TextureManager.h"
#include "Components.h"
#include "Systems.h"
#include "Constants.h"

#include <iostream>

namespace match3
{

Game::Game()
{
	constants = std::make_unique<constants::Constants>();
	constants->init("../data/constants.json");
}

Game::~Game()
{
}

void Game::init()
{
	int flags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		std::cout << SDL_GetError() << "\n";
		std::cout << "Subsystem Initialized.\n";

		window = SDL_CreateWindow(
			"Match3",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			constants->getScreenWidth(), constants->getScreenHeight(),
			flags);

		if (window) std::cout << "Window created!\n";

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_Event event;
			SDL_PollEvent(&event);
			std::cout << "Renderer created!\n";
		}
		isRunning = true;
	}
	else isRunning = false;

	auto& textureManager = textures::TextureManager::getInstance();
	textureManager.init(renderer, "../data/assets.json");

	systems.emplace_back(new RenderSystem());
	systems.emplace_back(new Match3System());

	for (auto& system : systems)
		system->init(registry, constants);
}

void Game::handleEvents()
{
}

void Game::update(double delta)
{
	for (auto& system : systems)
		system->update(delta);
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	for (auto& system : systems)
		system->render(registry, renderer);

	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned!\n";
}

bool Game::running()
{
	return isRunning;
}

}