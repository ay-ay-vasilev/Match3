#pragma once
#include "System.h"

class RenderSystem : public System
{
public:
	RenderSystem(entt::registry& registry, entt::dispatcher& dispatcher);
	// Interface
	void init(const constants::Constants& constants) override;
	void update(double delta) override;
	void render(SDL_Renderer* renderer) override;

private:
	// Members
	float scale{0.0};
};