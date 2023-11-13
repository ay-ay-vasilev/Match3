#include "Match3System.h"

#include "TextureManager.h"
#include "Components.h"
#include "Chips.h"
#include "Grid.h"
#include "Events.h"
#include "Constants.h"

#include <iostream>

Match3System::Match3System(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher) {}

Match3System::~Match3System()
{
	dispatcher.sink<events::ClickMatch3Event>().disconnect(this);
	dispatcher.sink<events::RetryEvent>().disconnect(this);
}

void Match3System::init(const constants::Constants& constants)
{
	dispatcher.sink<events::ClickMatch3Event>().connect<&Match3System::onClick>(this);
	dispatcher.sink<events::RetryEvent>().connect<&Match3System::onRetry>(this);

	grid = std::make_unique<match3::Grid>();
	grid->init(constants);
	grid->generateGrid();

	fillGridWithChipEntities();

	changeState(eGridTurnState::PLAYER_TURN);
}

void Match3System::update(double delta)
{
	switch (gridTurnState)
	{
	case eGridTurnState::INVALID:
	case eGridTurnState::PLAYER_TURN:
	case eGridTurnState::GAME_OVER:
		break;

	case eGridTurnState::UPDATE_SELECTED:
		updateSelected();
		changeState(eGridTurnState::SWAP_SELECTED);
		break;

	case eGridTurnState::SWAP_SELECTED:
		trySwapChips() ? changeState(eGridTurnState::CHECK_SWAP_MATCHES) : changeState(eGridTurnState::PLAYER_TURN);
		break;

	case eGridTurnState::CHECK_SWAP_MATCHES:
		checkSwapMatches() ? changeState(eGridTurnState::DESTROY_MATCHES) : changeState(eGridTurnState::SWAP_SELECTED_BACK);
		break;

	case eGridTurnState::SWAP_SELECTED_BACK:
		swapChips();
		resetSelected();
		changeState(eGridTurnState::PLAYER_TURN);
		break;

	case eGridTurnState::DESTROY_MATCHES:
		tryDestroyChips() ? changeState(eGridTurnState::SLIDE_CHIPS) : changeState(eGridTurnState::CHECK_VALID_SWAPS);
		break;

	case eGridTurnState::CHECK_VALID_SWAPS:
		checkValidSlots() ? changeState(eGridTurnState::PLAYER_TURN) : changeState(eGridTurnState::GAME_OVER);
		break;

	case eGridTurnState::SLIDE_CHIPS:
		slideChips();
		changeState(eGridTurnState::DESTROY_MATCHES);
		break;
	}
}

void Match3System::render(SDL_Renderer* renderer) {}

void Match3System::reset()
{
	changeState(eGridTurnState::INVALID);

	clearChipEntities();
	grid->reset();
	grid->generateGrid();
	fillGridWithChipEntities();

	changeState(eGridTurnState::PLAYER_TURN);
}

void Match3System::clearChipEntities()
{
	auto view = registry.view<ChipTag>();
	for (auto entity : view) {
		registry.destroy(entity);
	}
}

void Match3System::fillGridWithChipEntities()
{
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

void Match3System::updateSelected()
{
	const auto selected = grid->getSelectedCells();
	auto view = registry.view<GridCellTag, GridPositionComponent, SpriteComponent>();
	for (auto& entity : view)
	{
		auto& gridPosition = view.get<GridPositionComponent>(entity);
		auto textureName = "cellTexture";

		for (auto& pos : selected)
		{
			if (pos.first != gridPosition.row || pos.second != gridPosition.col) continue;

			textureName = "cellTextureSelected";
			break;
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
	grid->resetSelected();
}

bool Match3System::trySwapChips()
{
	if (!grid->hasSelectedPair())
		return false;
	
	swapChips();
	return true;
}

bool Match3System::checkSwapMatches()
{
	if (grid->hasChipsToDestroy())
	{
		resetSelected();
		return true;
	}

	changeState(eGridTurnState::SWAP_SELECTED_BACK);
	return false;
}

bool Match3System::checkValidSlots()
{
	return grid->hasValidSwaps();
}

void Match3System::swapChips()
{
	const auto selected = grid->getSelectedCells();
	std::vector<entt::entity> selectedChipEntities;

	auto chipView = registry.view<ChipTag>();

	for (auto& chipEntity : chipView)
	{
		auto& chipGridPosition = registry.get<GridPositionComponent>(chipEntity);
		if ((chipGridPosition.row == selected[0].first && chipGridPosition.col == selected[0].second) ||
			(chipGridPosition.row == selected[1].first && chipGridPosition.col == selected[1].second))
			selectedChipEntities.emplace_back(chipEntity);
	}

	assert(selectedChipEntities.size() == 2);

	auto& spriteComponent1 = registry.get<SpriteComponent>(selectedChipEntities[0]);
	auto& spriteComponent2 = registry.get<SpriteComponent>(selectedChipEntities[1]);
	std::swap(spriteComponent1, spriteComponent2);

	grid->swapSelected();

	grid->findAndMarkMatches(selected[0].first, selected[0].second);
	grid->findAndMarkMatches(selected[1].first, selected[1].second);
}

void Match3System::slideChips()
{
	const auto chipsToSlide = grid->getChipsToSlide();
	grid->slideChipsDown();
	slideChipEntities(chipsToSlide);
	checkSliddenChipMatches(chipsToSlide);
}

void Match3System::slideChipEntities(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide)
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

void Match3System::checkSliddenChipMatches(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide)
{
	int col = 0;
	for (const auto& chipsToSlideInCol : chipsToSlide)
	{
		for (const auto& chipToSlide : chipsToSlideInCol) grid->findAndMarkMatches(chipToSlide.second, col);
		++col;
	}
}

bool Match3System::tryDestroyChips()
{
	const auto chipsToDestroy = grid->destroyMarkedChips();

	if (chipsToDestroy.empty())
		return false;
	else
	{
		destroyChipEntities(chipsToDestroy);
		return true;
		resetSelected();
	}

}

void Match3System::destroyChipEntities(const std::vector<std::pair<int, int>>& chipsToDestroy)
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

	std::string chipTypeName = "chip_";
	if (grid->getGrid()[row][col]->hasBonus())
	{
		chipTypeName = grid->getGrid()[row][col]->getBonusName() + "_";
	}
	const auto& textureName = chipTypeName + grid->getGrid()[row][col]->getColorName();
	const auto& texture = textureManager.getTexture(textureName);
	const auto& textureRect = textureManager.getTextureRect(textureName);

	entt::entity entityChip = registry.create();
	registry.emplace<ChipTag>(entityChip);
	registry.emplace<TransformComponent>(entityChip, textureRect.w * startCol, textureRect.h * startRow, textureRect.w, textureRect.h);
	registry.emplace<SpriteComponent>(entityChip, textureRect, texture);
	registry.emplace<ClickableComponent>(entityChip);
	registry.emplace<GridPositionComponent>(entityChip, startRow, startCol);
}

void Match3System::changeState(eGridTurnState newGridTurnState)
{
	if (gridTurnState == newGridTurnState)
		return;

	gridTurnState = newGridTurnState;

	if (newGridTurnState == eGridTurnState::PLAYER_TURN)
		dispatcher.trigger(events::GridReadyEvent{});

	if (newGridTurnState == eGridTurnState::GAME_OVER)
		dispatcher.trigger(events::GameOverEvent{});
}

void Match3System::onClick(const events::ClickMatch3Event& event)
{
	auto view = registry.view<ChipTag>();

	for (auto& entity : view)
	{
		auto& transform = registry.get<TransformComponent>(entity);

		if (event.mouseX >= transform.x && event.mouseX <= transform.x + transform.w &&
			event.mouseY >= transform.y && event.mouseY <= transform.y + transform.h)
		{
			auto& clickable = registry.get<ClickableComponent>(entity);
			auto& gridPosition = registry.get<GridPositionComponent>(entity);

			grid->setSelectedCell(gridPosition.row, gridPosition.col);
			changeState(eGridTurnState::UPDATE_SELECTED);
			return;
		}
	}
	changeState(eGridTurnState::PLAYER_TURN);
}

void Match3System::onRetry()
{
	reset();
}