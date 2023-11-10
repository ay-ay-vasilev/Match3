#include "Game.h"

#include "Components.h"

#include <iostream>

namespace match3
{

SDL_Texture* loadTexture(const std::string& fileName, SDL_Renderer* renderer)
{
	std::string imagePath = "../";
	imagePath += fileName;

	SDL_Surface* tempSurface = IMG_Load(imagePath.c_str());
	if (tempSurface == nullptr)
	{
		throw std::runtime_error("Failed to load image: " + fileName + " Error: " + IMG_GetError());
	}
	auto texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	if (texture == nullptr)
	{
		throw std::runtime_error("Failed to create texture from surface: " + fileName + " Error: " + SDL_GetError());
	}
	SDL_FreeSurface(tempSurface);
	return texture;
}

Game::~Game()
{
	SDL_DestroyTexture(cellTexture);
	SDL_DestroyTexture(chipTexture);
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
			SCREEN_WIDTH, SCREEN_HEIGHT,
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

	cellTexture = loadTexture("assets/images/blue_square.png", renderer);
	chipTexture = loadTexture("assets/images/green_circle.png", renderer);

	textureRect = new SDL_Rect({ 0, 0, 32, 32 });

	for (int i = 0; i < LEVEL_SIZE; ++i)
	{
		for (int j = 0; j < LEVEL_SIZE; ++j)
		{
			entt::entity entityCell = registry.create();
			registry.emplace<TransformComponent>(entityCell, 32 * i * SCALE, 32 * j * SCALE, 32 * SCALE, 32 * SCALE);
			registry.emplace<SpriteComponent>(entityCell, textureRect, cellTexture);

			entt::entity entityChip = registry.create();
			registry.emplace<TransformComponent>(entityChip, 32 * i * SCALE, 32 * j * SCALE, 32 * SCALE, 32 * SCALE);
			registry.emplace<SpriteComponent>(entityChip, textureRect, chipTexture);
		}
	}
}

void Game::handleEvents()
{
}

void Game::update(double delta)
{
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	auto view = registry.view<TransformComponent, SpriteComponent>();
	for (auto entity : view)
	{
		const SDL_Rect source{ 0, 0, 32, 32 };
		auto& transformComponent = view.get<TransformComponent>(entity);
		auto& spriteComponent = view.get<SpriteComponent>(entity);
		const SDL_Rect destination{ transformComponent.x, transformComponent.y, transformComponent.w, transformComponent.h };
		SDL_RenderCopyEx(renderer, spriteComponent.texture, spriteComponent.textureRect, &destination, 0, NULL, SDL_FLIP_NONE);
	}

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