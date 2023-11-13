#pragma once
#include "System.h"

class RenderSystem : public System
{
public:
	RenderSystem(entt::registry& registry, entt::dispatcher& dispatcher);
	// Interface
	void init(const constants::Constants& constants, SDL_Renderer* renderer) override;
	void update(double delta) override;
	void render(SDL_Renderer* renderer) override;

private:
	// Members
	std::pair<int, int> offset{ 0, 0 };
	float scale{0.0};
};