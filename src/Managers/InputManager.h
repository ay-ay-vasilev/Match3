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
	void init(entt::dispatcher& dispatcher, const constants::Constants& constants);
	void handleEvent(SDL_Event& gameEvent, entt::dispatcher& dispatcher, const constants::Constants& constants);
	void handleMouseEvent(entt::dispatcher& dispatcher, const constants::Constants& constants);

	bool isExitGameEvent(SDL_Event& gameEvent);

private:
	// Events
	void onPlayerTurn();
	void onGridTurn();
	void onGameOver();
	void onMissclick();

	// Service
	void retry(entt::dispatcher& dispatcher);

	//Members
	std::pair<int, int> offset{ 0, 0 };
	int mouseX{ 0 };
	int mouseY{ 0 };
	bool blockClicks{ true };
	bool gameOver{ false };
};

}