#include "InputManager.h"

#include "Constants.h"
#include "Events.h"

#include <iostream>
#include <fstream>

namespace input
{

void InputManager::init(entt::dispatcher& dispatcher, const constants::Constants& constants)
{
	dispatcher.sink<events::PlayerTurnEvent>().connect<&InputManager::onPlayerTurn>(this);
	dispatcher.sink<events::GridTurnEvent>().connect<&InputManager::onGridTurn>(this);
	dispatcher.sink<events::GameOverEvent>().connect<&InputManager::onGameOver>(this);
	dispatcher.sink<events::MissclickEvent>().connect<&InputManager::onMissclick>(this);

	offset.first = (constants.getScreenWidth() - constants.getGridCellSize() * constants.getScale() * constants.getGridSize()) / 2;
	offset.second = (constants.getScreenHeight() - constants.getGridCellSize() * constants.getScale() * constants.getGridSize()) / 2;
}

void InputManager::handleEvent(SDL_Event& gameEvent, entt::dispatcher& dispatcher, const constants::Constants& constants)
{
	switch (gameEvent.type)
	{
	case SDL_KEYUP:
		if (gameOver || gameEvent.key.keysym.sym == SDLK_r) retry(dispatcher);
		break;

	case SDL_MOUSEBUTTONDOWN:
		gameOver ? retry(dispatcher) : handleMouseEvent(dispatcher, constants);
		break;
	default:
		break;
	}
}

void InputManager::handleMouseEvent(entt::dispatcher& dispatcher, const constants::Constants& constants)
{
	if (blockClicks) return;

	blockClicks = true;

	SDL_GetMouseState(&mouseX, &mouseY);

	int scaledMouseX = static_cast<int>((mouseX - offset.first) / constants.getScale());
	int scaledMouseY = static_cast<int>((mouseY - offset.second) / constants.getScale());

	dispatcher.trigger(events::ClickGameStateEvent{ scaledMouseX, scaledMouseY });
}

bool InputManager::isExitGameEvent(SDL_Event& gameEvent)
{
	switch (gameEvent.type)
	{
	case SDL_QUIT:
		return true;

	case SDL_KEYUP:
		if (gameEvent.key.keysym.sym == SDLK_ESCAPE)
			return true;
		break;
	}

	return false;
}

void InputManager::retry(entt::dispatcher& dispatcher)
{
	blockClicks = false;
	gameOver = false;

	dispatcher.trigger(events::RetryEvent{});
}

void InputManager::onPlayerTurn()
{
	blockClicks = false;
}

void InputManager::onGridTurn()
{
	blockClicks = true;
}

void InputManager::onGameOver()
{
	gameOver = true;
	blockClicks = true;
}

void InputManager::onMissclick()
{
	blockClicks = false;
}

}