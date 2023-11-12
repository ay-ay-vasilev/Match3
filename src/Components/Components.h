#pragma once
#include <SDL_image.h>
#include <iostream>

struct TransformComponent
{
	TransformComponent() = default;
	~TransformComponent() = default;
	TransformComponent(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {};

	int x{}, y{}, w{}, h{};
};

struct SpriteComponent
{
	SpriteComponent() = delete;
	SpriteComponent(SDL_Rect textureRect, SDL_Texture* texture) : textureRect(textureRect), texture(texture) {}

	void setTexture(SDL_Texture* newTexture) { texture = newTexture; }
	void setTextureRect(SDL_Rect newTextureRect) { textureRect = newTextureRect; }

	SDL_Rect textureRect{};
	SDL_Texture* texture{ nullptr };
};

struct ClickableComponent
{
	std::function<void()> onClick = []() { std::cout << "Clicked!\n"; };
};

struct GridPositionComponent
{
	GridPositionComponent() = delete;
	GridPositionComponent(int row, int col) : row(row), col(col) {}

	int row{}, col{};
};

// Tags
struct ChipTag {};
struct GridCellTag {};