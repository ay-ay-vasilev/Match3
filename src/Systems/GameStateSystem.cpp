#include "GameStateSystem.h"
#include "Components.h"

#include <iostream>

GameStateSystem::GameStateSystem(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher)
{
	dispatcher.sink<events::ClickGameStateEvent>().connect<&GameStateSystem::onClick>(this);
	dispatcher.sink<events::GridReadyEvent>().connect<&GameStateSystem::onGridReady>(this);
	dispatcher.sink<events::GameOverEvent>().connect<&GameStateSystem::onGameOver>(this);
}

GameStateSystem::~GameStateSystem()
{
	dispatcher.sink<events::ClickGameStateEvent>().disconnect(this);
	dispatcher.sink<events::GridReadyEvent>().disconnect(this);
}

void GameStateSystem::init(const constants::Constants& constants, SDL_Renderer* renderer) {}

void GameStateSystem::update(double delta) {}

void GameStateSystem::render(SDL_Renderer* renderer) {}

void GameStateSystem::changeState(eGameState newState)
{
	if (state == newState)
		return;

	state = newState;

	switch (state)
	{
	case eGameState::INVALID:
		break;
	case eGameState::PLAYER_TURN:
		dispatcher.trigger(events::PlayerTurnEvent{});
		break;
	case eGameState::GRID_TURN:
		dispatcher.trigger(events::GridTurnEvent{});
		break;
	case eGameState::GAME_OVER:
		break;
	}
}

void GameStateSystem::onClick(const events::ClickGameStateEvent& event)
{
	changeState(eGameState::GRID_TURN);

	dispatcher.trigger(events::ClickMatch3Event{ event.mouseX, event.mouseY});
}

void GameStateSystem::onGridReady()
{
	changeState(eGameState::PLAYER_TURN);
}

void GameStateSystem::onGameOver()
{
	changeState(eGameState::GAME_OVER);
}