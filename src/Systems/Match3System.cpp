#include "Match3System.h"

#include "TextureManager.h"
#include "Components.h"
#include "Chips.h"
#include "Grid.h"
#include "Events.h"
#include "Constants.h"

Match3System::Match3System(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher) {}

Match3System::~Match3System()
{
	dispatcher.sink<events::ClickGridEvent>().disconnect(this);
}

void Match3System::init(const constants::Constants& constants)
{
	dispatcher.sink<events::ClickGridEvent>().connect<&Match3System::onChipClicked>(this);

	grid = std::make_unique<match3::Grid>();
	grid->init(constants);
	grid->generateGrid();

	auto& textureManager = textures::TextureManager::getInstance();
	const auto& gridData = grid->getGrid();

	int gridRowNum = 0, gridColNum = 0;
	for (const auto& gridRow : gridData)
	{
		gridColNum = 0;
		for (const auto& gridCell : gridRow)
		{
			addGridCellEntity(gridRowNum, gridColNum);
			if (gridCell->hasColor()) addGridChipEntity(gridRowNum, gridColNum);

			++gridColNum;
		}
		++gridRowNum;
	}
}

void Match3System::update(double delta)
{
	if (isNeedUpdateSelected)
	{
		updateSelected();
		isNeedUpdateSelected = false;
		return;
	}
	if (isNeedSlideChipsDown)
	{
		const auto chipsToSlide = grid->getChipsToSlide();
		grid->slideChipsDown();

		slideChips(chipsToSlide);

		checkSliddenChips(chipsToSlide);

		isNeedSlideChipsDown = false;
		isNeedToCheckForCombos = true;
		return;
	}
	if (isNeedToCheckForCombos)
	{
		const auto chipsToDestroy = grid->destroyMarkedChips();

		if (chipsToDestroy.empty())
			isNeedSlideChipsDown = false;
		else
		{
			resetSelected();
			isNeedSlideChipsDown = true;
		}

		destroyChips(chipsToDestroy);
		return;
	}
}

void Match3System::render(SDL_Renderer* renderer) {}

void Match3System::updateSelected()
{
	const auto selected = grid->getSelectedCells();
	std::vector<entt::entity> selectedCells;

	auto view = registry.view<GridCellTag, GridPositionComponent, SpriteComponent>();
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
			swapChips(selected);
			selectedCells.clear();
		}

		auto& spriteComponent = view.get<SpriteComponent>(entity);
		auto& textureManager = textures::TextureManager::getInstance();

		const auto& texture = textureManager.getTexture(textureName);
		const auto& textureRect = textureManager.getTextureRect(textureName);

		spriteComponent.setTexture(texture);
		spriteComponent.setTextureRect(textureRect);
	}
}

void Match3System::resetSelected()
{
	auto view = registry.view<GridCellTag, GridPositionComponent, SpriteComponent>();
	for (auto& entity : view)
	{
		auto& gridPosition = view.get<GridPositionComponent>(entity);
		auto textureName = "cellTexture";

		auto& spriteComponent = view.get<SpriteComponent>(entity);
		auto& textureManager = textures::TextureManager::getInstance();

		const auto& texture = textureManager.getTexture(textureName);
		const auto& textureRect = textureManager.getTextureRect(textureName);

		spriteComponent.setTexture(texture);
		spriteComponent.setTextureRect(textureRect);
	}
}

void Match3System::swapChips(const std::array<std::pair<int, int>, 2>& selected)
{
	std::vector<entt::entity> selectedChips;

	auto chipView = registry.view<ChipTag>();

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

	grid->findAndMarkCombo(selected[0].first, selected[0].second);
	grid->findAndMarkCombo(selected[1].first, selected[1].second);

	isNeedToCheckForCombos = true;
}

void Match3System::slideChips(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide)
{
	int col = 0;
	// add newly generated chip entities
	for (const auto& chipsToSlideInCol : chipsToSlide)
	{
		for (const auto& chipToSlide : chipsToSlideInCol)
		{
			if (chipToSlide.first < 0)
			{
				int startRow = chipToSlide.first;
				int row = chipToSlide.second;
				addGridChipEntity(row, col, startRow, col);
			}
		}
		++col;
	}

	// slide chips - todo with animations
	auto chipView = registry.view<ChipTag>();

	for (auto& chipEntity : chipView)
	{
		auto& chipGridPosition = registry.get<GridPositionComponent>(chipEntity);

		for (auto& chipToSlide : chipsToSlide[chipGridPosition.col])
		{
			if (chipToSlide.first == chipGridPosition.row)
			{
				auto& chipSprite = registry.get<SpriteComponent>(chipEntity);
				auto& chipTransform = registry.get<TransformComponent>(chipEntity);

				chipTransform.y = chipSprite.textureRect.h * chipToSlide.second;
				chipGridPosition.row = chipToSlide.second;
			}
		}
	}
}

void Match3System::checkSliddenChips(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide)
{
	int col = 0;
	for (const auto& chipsToSlideInCol : chipsToSlide)
	{
		for (const auto& chipToSlide : chipsToSlideInCol)
			grid->findAndMarkCombo(chipToSlide.second, col);

		++col;
	}
}

void Match3System::destroyChips(const std::vector<std::pair<int, int>>& chipsToDestroy)
{
	auto view = registry.view<ChipTag, GridPositionComponent>();
	for (auto& entity : view)
	{
		auto& gridPosition = view.get<GridPositionComponent>(entity);
		bool found{ false };
		for (auto& chipPos : chipsToDestroy)
		{
			if (chipPos.first != gridPosition.row || chipPos.second != gridPosition.col)
				continue;
			found = true;
			break;
		}

		if (!found) continue;

		registry.destroy(entity);
	}
}

void Match3System::addGridCellEntity(int row, int col)
{
	auto& textureManager = textures::TextureManager::getInstance();

	const auto& textureName = "cellTexture";
	const auto& texture = textureManager.getTexture(textureName);
	const auto& textureRect = textureManager.getTextureRect(textureName);

	entt::entity entityCell = registry.create();
	registry.emplace<GridCellTag>(entityCell);
	registry.emplace<TransformComponent>(entityCell, textureRect.w * col, textureRect.h * row, textureRect.w, textureRect.h);
	registry.emplace<SpriteComponent>(entityCell, textureRect, texture);
	registry.emplace<GridPositionComponent>(entityCell, row, col);
}

void Match3System::addGridChipEntity(int row, int col)
{
	addGridChipEntity(row, col, row, col);
}

void Match3System::addGridChipEntity(int row, int col, int startRow, int startCol)
{
	auto& textureManager = textures::TextureManager::getInstance();

	const auto& textureName = "chip_" + grid->getGrid()[row][col]->getColorName();
	const auto& texture = textureManager.getTexture(textureName);
	const auto& textureRect = textureManager.getTextureRect(textureName);

	entt::entity entityChip = registry.create();
	registry.emplace<ChipTag>(entityChip);
	registry.emplace<TransformComponent>(entityChip, textureRect.w * startCol, textureRect.h * startRow, textureRect.w, textureRect.h);
	registry.emplace<SpriteComponent>(entityChip, textureRect, texture);
	registry.emplace<ClickableComponent>(entityChip);
	registry.emplace<GridPositionComponent>(entityChip, startRow, startCol);
}

void Match3System::onChipClicked(const events::ClickGridEvent& event)
{
	grid->setSelectedCell(event.row, event.col);
	isNeedUpdateSelected = true;
}