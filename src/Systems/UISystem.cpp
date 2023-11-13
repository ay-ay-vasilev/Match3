#include "UISystem.h"
#include "Components.h"
#include "TextureManager.h"

#include "Events.h"

UISystem::UISystem(entt::registry& registry, entt::dispatcher& dispatcher) : System(registry, dispatcher) {}

UISystem::~UISystem()
{
	dispatcher.sink<events::GameOverEvent>().disconnect(this);
	dispatcher.sink<events::RetryEvent>().disconnect(this);
}

void UISystem::init(const constants::Constants& constants, SDL_Renderer* renderer)
{
	dispatcher.sink<events::GameOverEvent>().connect<&UISystem::onGameOver>(this);
	dispatcher.sink<events::RetryEvent>().connect<&UISystem::onRetry>(this);

	entt::entity entity = registry.create();

	SDL_Rect position{ constants.getScreenWidth() / 2, constants.getScreenHeight() / 2};
	SDL_Color textColor{ 255, 0, 0, 255 };

	auto& labelComponent = registry.emplace<UILabelComponent>(entity, position, textColor);
	labelComponent.setLabeLText("GAME OVER", renderer, 64);
	SDL_Rect textRect =
	{
		labelComponent.position.x - labelComponent.position.w / 2,
		labelComponent.position.y - labelComponent.position.h / 2,
		labelComponent.position.w, labelComponent.position.h
	};
	labelComponent.position = textRect;

	auto& textureManager = textures::TextureManager::getInstance();
	blackTexture = textureManager.getTexture("blackTexture");
	blackTextureSrcRect = { 0, 0, 1, 1 };
	blackTextureDestRect = { 0, 0, constants.getScreenWidth(), constants.getScreenHeight()};
}

void UISystem::update(double delta) {}

void UISystem::render(SDL_Renderer* renderer)
{
	if (!gameOver) return;

	auto view = registry.view<UILabelComponent>();
	for (auto entity : view)
	{
		auto& labelComponent = view.get<UILabelComponent>(entity);

		SDL_SetTextureAlphaMod(blackTexture, 200);
		SDL_RenderCopyEx(renderer, blackTexture, &blackTextureSrcRect, &blackTextureDestRect, 0, NULL, SDL_FLIP_NONE);


		SDL_RenderCopy(renderer, labelComponent.labelTexture, nullptr, &labelComponent.position);
	}
}

void UISystem::onGameOver()
{
	gameOver = true;
}

void UISystem::onRetry()
{
	gameOver = false;
}