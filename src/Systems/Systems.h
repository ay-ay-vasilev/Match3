#pragma once
#include "Components.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Grid.h"
#include "Chips.h"

#include <entt/entt.hpp>
#include <memory>

class System
{
public:
	// Interface
	virtual void init(entt::registry& registry, const std::unique_ptr<constants::Constants>& constants) = 0;
	virtual void render(entt::registry& registry, SDL_Renderer* renderer) = 0;
	virtual void update(double delta) = 0;
};

class RenderSystem : public System
{
public:
	// Interface
	void init(entt::registry& registry, const std::unique_ptr<constants::Constants>& constants) override
	{
		scale = constants->getScale();
	}

	void update(double delta) override {}
	void render(entt::registry& registry, SDL_Renderer* renderer) override
	{
		auto view = registry.view<TransformComponent, SpriteComponent>();
		for (auto entity : view)
		{
			auto& transformComponent = view.get<TransformComponent>(entity);
			auto& spriteComponent = view.get<SpriteComponent>(entity);
			const SDL_Rect destination{ transformComponent.x * scale, transformComponent.y * scale, transformComponent.w * scale, transformComponent.h * scale };
			SDL_RenderCopyEx(renderer, spriteComponent.texture, &spriteComponent.textureRect, &destination, 0, NULL, SDL_FLIP_NONE);
		}
	}

private:
	double scale{0.0};
};

class Match3System : public System
{
public:
	// Interface
	void init(entt::registry& registry, const std::unique_ptr<constants::Constants>& constants) override
	{
		grid = std::make_unique<match3::Grid>();
		grid->generate(constants);

		auto& textureManager = textures::TextureManager::getInstance();
		const auto& gridData = grid->getGrid();

		int gridRowNum = 0, gridColNum = 0;
		for (const auto& gridRow : gridData)
		{
			gridColNum = 0;
			for (const auto& gridCell : gridRow)
			{
				{
					const auto& textureName = "cellTexture";
					const auto& texture = textureManager.getTexture(textureName);
					const auto& textureRect = textureManager.getTextureRect(textureName);

					entt::entity entityCell = registry.create();
					registry.emplace<TransformComponent>(entityCell, textureRect.w * gridRowNum, textureRect.h * gridColNum, textureRect.w, textureRect.h);
					registry.emplace<SpriteComponent>(entityCell, textureRect, texture);
				}

				if (gridCell->hasColor())
				{
					const auto& textureName = "chip_" + gridCell->getColorName();
					const auto& texture = textureManager.getTexture(textureName);
					const auto& textureRect = textureManager.getTextureRect(textureName);

					entt::entity entityChip = registry.create();
					registry.emplace<TransformComponent>(entityChip, textureRect.w * gridRowNum, textureRect.h * gridColNum, textureRect.w, textureRect.h);
					registry.emplace<SpriteComponent>(entityChip, textureRect, texture);
				}
				++gridColNum;
			}
			++gridRowNum;
		}
	}

	void update(double delta) override {}
	void render(entt::registry& registry, SDL_Renderer* renderer) override {}
private:
	// Members
	std::unique_ptr<match3::Grid> grid;
};