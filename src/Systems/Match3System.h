#pragma once
#include "System.h"

#include <memory>

namespace match3
{
	class Grid;
}

namespace events
{
	struct ClickMatch3Event;
}

namespace constants
{
	class Constants;
}

enum class eGridTurnState
{
	INVALID,
	PLAYER_TURN,
	UPDATE_SELECTED,
	SWAP_SELECTED,
	CHECK_SWAP_COMBOS,
	SWAP_SELECTED_BACK,
	DESTROY_COMBOS,
	CHECK_VALID_SWAPS,
	SLIDE_CHIPS,
	GAME_OVER
};

class Match3System : public System
{
public:
	Match3System(entt::registry& registry, entt::dispatcher& dispatcher);
	~Match3System() override;

	// Interface
	void init(const constants::Constants& constants) override;
	void update(double delta) override;
	void render(SDL_Renderer* renderer) override;

private:
	// Service
	void reset();
	void updateSelected();
	void resetSelected();
	bool trySwapChips();
	bool checkSwapCombos();
	bool checkValidSlots();
	void swapChips();
	void slideChips();
	void checkSliddenChipCombos(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide);
	bool tryDestroyChips();
	void changeState(eGridTurnState newGridTurnState);

	// Service - Entity Management
	void slideChipEntities(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide);
	void destroyChipEntities(const std::vector<std::pair<int, int>>& chipsToDestroy);
	void clearChipEntities();
	void fillGridWithChipEntities();
	void addGridCellEntity(int row, int col);
	void addGridChipEntity(int row, int col);
	void addGridChipEntity(int row, int col, int startRow, int startCol); // todo: fix

	// Events
	void onClick(const events::ClickMatch3Event& event);
	void onRetry();

	// Members
	eGridTurnState gridTurnState{ eGridTurnState::INVALID };
	std::unique_ptr<match3::Grid> grid;
};