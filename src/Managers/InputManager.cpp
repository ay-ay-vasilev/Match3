#include "InputManager.h"

#include "Components.h"
#include "Constants.h"
#include "Events.h"

#include <iostream>
#include <fstream>

namespace input
{

void InputManager::handleMouseEvent(entt::registry& registry, entt::dispatcher& dispatcher, const constants::Constants& constants)
{
	SDL_GetMouseState(&mouseX, &mouseY);

	auto view = registry.view<ChipComponent>();

	for (auto& entity : view)
	{
		auto& transform = registry.get<TransformComponent>(entity);

		int scaledWidth = static_cast<int>(transform.w * constants.getScale());
		int scaledHeight = static_cast<int>(transform.h * constants.getScale());

		int scaledX = static_cast<int>(transform.x * constants.getScale());
		int scaledY = static_cast<int>(transform.y * constants.getScale());

		if (mouseX >= scaledX && mouseX <= scaledX + scaledWidth &&
			mouseY >= scaledY && mouseY <= scaledY + scaledHeight)
		{
			auto& clickable = registry.get<ClickableComponent>(entity);
			auto& gridPosition = registry.get<GridPositionComponent>(entity);

			dispatcher.trigger(events::ClickGridEvent{ gridPosition.row, gridPosition.col });
		}
	}
}

}