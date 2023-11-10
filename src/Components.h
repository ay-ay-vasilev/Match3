#pragma once
#include <SDL_image.h>

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
	SpriteComponent(SDL_Rect* textureRect, SDL_Texture* texture) : textureRect(textureRect), texture(texture) {}

	SDL_Rect* textureRect{};
	SDL_Texture* texture{ nullptr };
};