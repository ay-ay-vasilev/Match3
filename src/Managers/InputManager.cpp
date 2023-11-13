#include "InputManager.h"

#include "Constants.h"
#include "Events.h"

#include <iostream>
#include <fstream>

namespace input
{

void InputManager::init(entt::dispatcher& dispatcher)
{
	dispatcher.sink<events::PlayerTurnEvent>().connect<&InputManager::onPlayerTurn>(this);
	dispatcher.sink<events::GridTurnEvent>().connect<&InputManager::onGridTurn>(this);
}

void InputManager::handleMouseEvent(entt::dispatcher& dispatcher, const constants::Constants& constants)
{
	if (blockClicks) return;

	SDL_GetMouseState(&mouseX, &mouseY);

	int scaledMouseX = static_cast<int>(mouseX / constants.getScale());
	int scaledMouseY = static_cast<int>(mouseY / constants.getScale());

	dispatcher.trigger(events::ClickGameStateEvent{ scaledMouseX, scaledMouseY });

	blockClicks = true;
}

void InputManager::onPlayerTurn()
{
	blockClicks = false;
}

void InputManager::onGridTurn()
{
	blockClicks = true;
}

}