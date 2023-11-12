#pragma once
#include "Constants.h"
#include <SDL_image.h>
#include <entt/entt.hpp>
#include <memory>

class System
{
public:
	System() = delete;
	System(entt::registry& registry, entt::dispatcher& dispatcher) : registry(registry), dispatcher(dispatcher) {}
	virtual ~System() {};
	// Interface
	virtual void init(const constants::Constants& constants) = 0;
	virtual void render(SDL_Renderer* renderer) = 0;
	virtual void update(double delta) = 0;

protected:
	entt::registry& registry;
	entt::dispatcher& dispatcher;
};