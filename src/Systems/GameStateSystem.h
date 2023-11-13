#pragma once
#include "System.h"

#include "Events.h"

enum class eGameState
{
	INVALID,
	PLAYER_TURN,
	GRID_TURN,
	GAME_OVER
};

class GameStateSystem : public System
{
public:
	GameStateSystem(entt::registry& registry, entt::dispatcher& dispatcher);
	~GameStateSystem() override;

	// Interface
	void init(const constants::Constants& constants, SDL_Renderer* renderer) override;
	void update(double delta) override;
	void render(SDL_Renderer* renderer) override;

	// Events
	void onClick(const events::ClickGameStateEvent& event);
	void onGridReady();
	void onGameOver();

private:
	// Service
	void changeState(eGameState newState);

	// Members
	eGameState state = eGameState::INVALID;

};