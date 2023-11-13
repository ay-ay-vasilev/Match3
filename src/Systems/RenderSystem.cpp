#include "RenderSystem.h"
#include "Components.h"

RenderSystem::RenderSystem(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher) {}

void RenderSystem::init(const constants::Constants& constants, SDL_Renderer* renderer)
{
	scale = constants.getScale();
	offset.first = (constants.getScreenWidth() - constants.getGridCellSize() * scale * constants.getGridSize()) / 2;
	offset.second = (constants.getScreenHeight() - constants.getGridCellSize() * scale * constants.getGridSize()) / 2;
}

void RenderSystem::update(double delta) {}

void RenderSystem::render(SDL_Renderer* renderer)
{
	auto view = registry.view<TransformComponent, SpriteComponent>();
	for (auto entity : view)
	{
		auto& transformComponent = view.get<TransformComponent>(entity);
		auto& spriteComponent = view.get<SpriteComponent>(entity);
		const SDL_Rect destination
		{
			static_cast<int>(static_cast<float>(transformComponent.x) * scale) + offset.first,
			static_cast<int>(static_cast<float>(transformComponent.y) * scale) + offset.second,
			static_cast<int>(static_cast<float>(transformComponent.w) * scale),
			static_cast<int>(static_cast<float>(transformComponent.h) * scale)
		};
		SDL_RenderCopyEx(renderer, spriteComponent.texture, &spriteComponent.textureRect, &destination, 0, NULL, SDL_FLIP_NONE);
	}
}