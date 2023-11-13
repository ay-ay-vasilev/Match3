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
	SLIDE_CHIPS
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
	void updateSelected();
	void resetSelected();
	bool trySwapChips();
	bool checkSwapCombos();
	void swapChips();
	void slideChips();
	void slideChipEntities(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide);
	void checkSliddenChipCombos(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide);
	bool tryDestroyChips();
	void destroyChips(const std::vector<std::pair<int, int>>& chipsToDestroy);

	void addGridCellEntity(int row, int col);
	void addGridChipEntity(int row, int col);
	void addGridChipEntity(int row, int col, int startRow, int startCol); // todo: fix

	void changeState(eGridTurnState newGridTurnState);

	// Events
	void onClick(const events::ClickMatch3Event& event);

	// Members
	eGridTurnState gridTurnState{ eGridTurnState::INVALID };
	std::unique_ptr<match3::Grid> grid;
};