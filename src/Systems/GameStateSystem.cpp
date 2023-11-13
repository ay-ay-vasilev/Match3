#include "GameStateSystem.h"
#include "Components.h"

GameStateSystem::GameStateSystem(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher)
{
	dispatcher.sink<events::ClickGameStateEvent>().connect<&GameStateSystem::onClick>(this);
	dispatcher.sink<events::GridReadyEvent>().connect<&GameStateSystem::onGridReady>(this);
}

GameStateSystem::~GameStateSystem()
{
	dispatcher.sink<events::ClickGameStateEvent>().disconnect(this);
	dispatcher.sink<events::GridReadyEvent>().disconnect(this);
}

void GameStateSystem::init(const constants::Constants& constants) {}

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
	}
}

void GameStateSystem::onClick(const events::ClickGameStateEvent& event)
{
	dispatcher.trigger(events::ClickMatch3Event{ event.mouseX, event.mouseY});

	changeState(eGameState::GRID_TURN);
}

void GameStateSystem::onGridReady()
{
	changeState(eGameState::PLAYER_TURN);
}