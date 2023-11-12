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
	void swapChips(const std::array<std::pair<int, int>, 2>& selected);

	// Events
	void onChipClicked(const events::ClickGridEvent& event);

	// Members
	std::unique_ptr<match3::Grid> grid;
	bool isNeedUpdateSelected{ false };
};