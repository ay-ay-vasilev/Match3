#pragma once
#include <SDL_image.h>
#include <json.hpp>
#include <unordered_map>
#include <string>

namespace textures
{

SDL_Texture* loadTexture(const std::string& fileName, SDL_Renderer* renderer);
SDL_Surface* loadSurface(const std::string& fileName);

struct TextureStruct
{
	SDL_Rect textureRect;
	SDL_Texture* texture;
};

class TextureManager
{
public:
	static TextureManager& getInstance()
	{
		static TextureManager instance;
		return instance;
	}

private:
	TextureManager() {}
	~TextureManager()
	{
		for (auto& element : textureMap)
		{
			SDL_DestroyTexture(element.second.texture);
		}
	}

public:
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator=(TextureManager&&) = delete;

	// Interface
	void init(SDL_Renderer* renderer, const std::string fileName);

	const TextureStruct& getTextureStruct(const std::string id) const;
	SDL_Texture* getTexture(const std::string id) const;
	const SDL_Rect& getTextureRect(const std::string id) const;

private:
	// Service
	void loadFromJsonObject(const nlohmann::json& constantsJson);

	SDL_Renderer* renderer{ nullptr };
	std::unordered_map<std::string, TextureStruct> textureMap;

};

}