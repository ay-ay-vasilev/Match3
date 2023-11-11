#pragma once
#include <SDL_image.h>
#include <unordered_map>
#include <string>

namespace textures
{

SDL_Texture* loadTexture(const std::string& fileName, SDL_Renderer* renderer);
SDL_Surface* loadSurface(const std::string& fileName);

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
			SDL_DestroyTexture(element.second);
		}
	}

public:
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager(TextureManager&&) = delete;
	TextureManager& operator=(TextureManager&&) = delete;

	// Interface
	void init(SDL_Renderer* renderer);

	SDL_Texture* getTexture(const std::string id) const;

private:
	SDL_Renderer* renderer{ nullptr };
	std::unordered_map<std::string, SDL_Texture*> textureMap;

};

}