#pragma once
#include <SDL_image.h>
#include <SDL_ttf.h>
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

struct UILabelComponent
{
	UILabelComponent() = delete;
	UILabelComponent(SDL_Rect position, SDL_Color textColor) : position(position), textColor(textColor) {}
	~UILabelComponent()
	{
		SDL_DestroyTexture(labelTexture);
	}

	void setLabeLText(std::string text, SDL_Renderer* renderer, int textSize)
	{
		if (labelTexture) SDL_DestroyTexture(labelTexture);
		const std::string fontPath = "../assets/fonts/arial.ttf";
		SDL_Surface* surf = TTF_RenderText_Blended(TTF_OpenFont(fontPath.c_str(), textSize), text.c_str(), textColor);
		labelTexture = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
		SDL_QueryTexture(labelTexture, nullptr, nullptr, &position.w, &position.h);
	}

	SDL_Rect position{ 0, 0, 0, 0 };
	SDL_Color textColor{ 255, 255, 255, 255 };
	SDL_Texture* labelTexture{ nullptr };
};

// Tags
struct ChipTag {};
struct GridCellTag {};