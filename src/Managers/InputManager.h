#pragma once
#include <SDL.h>
#include <entt/entt.hpp>
#include <memory>

namespace constants
{
	class Constants;
}

namespace input
{

class InputManager
{
public:
	static InputManager& getInstance()
	{
		static InputManager instance;
		return instance;
	}

private:
	InputManager() {};

public:
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// Interface
	void init(entt::dispatcher& dispatcher);
	void handleMouseEvent(entt::dispatcher& dispatcher, const constants::Constants& constants);

private:
	// Events
	void onPlayerTurn();
	void onGridTurn();

	// Service

	//Members
	int mouseX{ 0 };
	int mouseY{ 0 };
	bool blockClicks{ true };
};

}