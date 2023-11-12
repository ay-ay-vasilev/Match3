#pragma once
#include "System.h"

#include <memory>

namespace match3
{
	class Grid;
}

namespace events
{
	struct ClickGridEvent;
}

namespace constants
{
	class Constants;
}

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
	void swapChips(const std::array<std::pair<int, int>, 2>& selected);
	void slideChips(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide);
	void checkSliddenChips(const std::vector<std::vector<std::pair<int, int>>>& chipsToSlide);
	void destroyChips(const std::vector<std::pair<int, int>>& chipsToDestroy);

	void addGridCellEntity(int row, int col);
	void addGridChipEntity(int row, int col);
	void addGridChipEntity(int row, int col, int startRow, int startCol); // todo: fix

	// Events
	void onChipClicked(const events::ClickGridEvent& event);

	// Members
	std::unique_ptr<match3::Grid> grid;
	bool isNeedUpdateSelected{ false };
	bool isNeedSlideChipsDown{ false };
	bool isNeedToCheckForCombos{ false };
};