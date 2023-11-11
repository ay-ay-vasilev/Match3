#pragma once
#include "Components.h"
#include "Events.h"
#include "Constants.h"
#include "TextureManager.h"
#include "Grid.h"
#include "Chips.h"

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

class RenderSystem : public System
{
public:
	RenderSystem(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher) {}
	// Interface
	void init(const constants::Constants& constants) override
	{
		scale = constants.getScale();
	}

	void update(double delta) override {}
	void render(SDL_Renderer* renderer) override
	{
		auto view = registry.view<TransformComponent, SpriteComponent>();
		for (auto entity : view)
		{
			auto& transformComponent = view.get<TransformComponent>(entity);
			auto& spriteComponent = view.get<SpriteComponent>(entity);
			const SDL_Rect destination
			{
				static_cast<int>(static_cast<float>(transformComponent.x) * scale),
				static_cast<int>(static_cast<float>(transformComponent.y) * scale),
				static_cast<int>(static_cast<float>(transformComponent.w) * scale),
				static_cast<int>(static_cast<float>(transformComponent.h) * scale)
			};
			SDL_RenderCopyEx(renderer, spriteComponent.texture, &spriteComponent.textureRect, &destination, 0, NULL, SDL_FLIP_NONE);
		}
	}

private:
	float scale{0.0};
};

class Match3System : public System
{
public:
	Match3System(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher) {}
	~Match3System() override
	{
		dispatcher.sink<events::ClickGridEvent>().disconnect(this);
	}
	// Interface
	void init(const constants::Constants& constants) override
	{
		dispatcher.sink<events::ClickGridEvent>().connect<&Match3System::onChipClicked>(this);

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
					registry.emplace<GridCellComponent>(entityCell);
					registry.emplace<TransformComponent>(entityCell, textureRect.w * gridColNum, textureRect.h * gridRowNum, textureRect.w, textureRect.h);
					registry.emplace<SpriteComponent>(entityCell, textureRect, texture);
					registry.emplace<GridPositionComponent>(entityCell, gridRowNum, gridColNum);
				}

				if (gridCell->hasColor())
				{
					const auto& textureName = "chip_" + gridCell->getColorName();
					const auto& texture = textureManager.getTexture(textureName);
					const auto& textureRect = textureManager.getTextureRect(textureName);

					entt::entity entityChip = registry.create();
					registry.emplace<ChipComponent>(entityChip);
					registry.emplace<TransformComponent>(entityChip, textureRect.w * gridColNum, textureRect.h * gridRowNum, textureRect.w, textureRect.h);
					registry.emplace<SpriteComponent>(entityChip, textureRect, texture);
					registry.emplace<ClickableComponent>(entityChip);
					registry.emplace<GridPositionComponent>(entityChip, gridRowNum, gridColNum);
				}
				++gridColNum;
			}
			++gridRowNum;
		}
	}

	void update(double delta) override
	{
		if (isNeedUpdateSelected)
		{
			const auto& selected = grid->getSelectedCells();
			std::vector<entt::entity> selectedCells;

			auto view = registry.view<GridCellComponent, GridPositionComponent, SpriteComponent>();
			for (auto& entity : view)
			{
				auto& gridPosition = view.get<GridPositionComponent>(entity);
				auto textureName = "cellTexture";

				for (auto& pos : selected)
				{
					if (pos.first != gridPosition.row || pos.second != gridPosition.col) continue;
					
					textureName = "cellTextureSelected";
					selectedCells.emplace_back(entity);
					break;
				}

				if (selectedCells.size() == 2)
				{
					std::vector<entt::entity> selectedChips;

					auto chipView = registry.view<ChipComponent>();

					for (auto& chipEntity : chipView)
					{
						auto& chipGridPosition = registry.get<GridPositionComponent>(chipEntity);
						if ((chipGridPosition.row == selected[0].first && chipGridPosition.col == selected[0].second) ||
							(chipGridPosition.row == selected[1].first && chipGridPosition.col == selected[1].second))
							selectedChips.emplace_back(chipEntity);
					}

					if (selectedChips.size() == 2)
					{
						auto& spriteComponent1 = registry.get<SpriteComponent>(selectedChips[0]);
						auto& spriteComponent2 = registry.get<SpriteComponent>(selectedChips[1]);

						std::swap(spriteComponent1, spriteComponent2);
					}

					grid->swapSelected();
					selectedCells.clear();
				}

				auto& spriteComponent = view.get<SpriteComponent>(entity);
				auto& textureManager = textures::TextureManager::getInstance();

				const auto& texture = textureManager.getTexture(textureName);
				const auto& textureRect = textureManager.getTextureRect(textureName);

				spriteComponent.setTexture(texture);
				spriteComponent.setTextureRect(textureRect);
			}
			isNeedUpdateSelected = false;
		}
	}

	void render(SDL_Renderer* renderer) override {}

private:
	// Events
	void onChipClicked(const events::ClickGridEvent& event)
	{
		grid->setSelectedCell(event.row, event.col);
		isNeedUpdateSelected = true;
	}

	// Members
	std::unique_ptr<match3::Grid> grid;
	bool isNeedUpdateSelected{ false };
};