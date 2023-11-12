#include "Game.h"

#include <SDL_ttf.h>

#include "TextureManager.h"
#include "InputManager.h"
#include "Components.h"
#include "RenderSystem.h"
#include "Match3System.h"
#include "Constants.h"

#include <iostream>

Game::Game()
{
	constants.init("../data/constants.json");
}

Game::~Game() {}

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
			constants.getScreenWidth(), constants.getScreenHeight(),
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

	if (TTF_Init() == -1) std::cout << "Error: SDL_TTF\n";

	auto& textureManager = textures::TextureManager::getInstance();
	textureManager.init(renderer, "../data/assets.json");

	auto& inputManager = input::InputManager::getInstance();

	systems.emplace_back(new RenderSystem(registry, dispatcher));
	systems.emplace_back(new Match3System(registry, dispatcher));

	for (auto& system : systems)
		system->init(constants);
}

void Game::handleEvents()
{
	while (SDL_PollEvent(&gameEvent) != 0)
	{
		switch (gameEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (gameEvent.key.keysym.sym == SDLK_ESCAPE)
				isRunning = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			input::InputManager::getInstance().handleMouseEvent(registry, dispatcher, constants);
			break;
		default:
			break;
		}
	}
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
		system->render(renderer);

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