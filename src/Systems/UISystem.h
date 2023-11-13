#pragma once
#include "System.h"
#include <SDL_image.h>

class UISystem : public System
{
public:
	UISystem(entt::registry& registry, entt::dispatcher& dispatcher);
	~UISystem() override;

	// Interface
	void init(const constants::Constants& constants, SDL_Renderer* renderer) override;
	void update(double delta) override;
	void render(SDL_Renderer* renderer) override;

private:

	// Events
	void onGameOver();
	void onRetry();

	// Members
	bool gameOver{ false };

	SDL_Texture* blackTexture{ nullptr };
	SDL_Rect blackTextureSrcRect{ 0, 0, 0, 0 };
	SDL_Rect blackTextureDestRect{ 0, 0, 0, 0 };
};