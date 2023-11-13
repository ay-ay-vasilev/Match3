#include "TextureManager.h"

#include <iostream>
#include <fstream>

namespace textures
{

SDL_Texture* loadTexture(const std::string& fileName, SDL_Renderer* renderer)
{
	std::string imagePath = "../";
	imagePath += fileName;

	SDL_Surface* tempSurface = IMG_Load(imagePath.c_str());
	if (tempSurface == nullptr)
	{
		throw std::runtime_error("Failed to load image: " + fileName + " Error: " + IMG_GetError());
	}
	auto texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	if (texture == nullptr)
	{
		throw std::runtime_error("Failed to create texture from surface: " + fileName + " Error: " + SDL_GetError());
	}
	SDL_FreeSurface(tempSurface);
	return texture;
}

SDL_Surface* loadSurface(const std::string& fileName)
{
	std::string imagePath = "../";
	imagePath += fileName;

	SDL_Surface* surface = IMG_Load(imagePath.c_str());
	if (surface == nullptr)
	{
		throw std::runtime_error("Failed to load image: " + fileName + " Error: " + IMG_GetError());
	}
	return surface;
}

void TextureManager::init(SDL_Renderer* renderer, const std::string fileName)
{
	this->renderer = renderer;

	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cerr << "Failed to open assets file: " << fileName << std::endl;
		return;
	}

	nlohmann::json assetsJson;
	file >> assetsJson;
	loadFromJsonObject(assetsJson);
}

const TextureStruct& TextureManager::getTextureStruct(const std::string id) const
{
	auto it = textureMap.find(id);
	if (it != textureMap.end())
		return it->second;

	std::cout << "Error: texture not found (" + id + ").\n";
	return {};
}

SDL_Texture* TextureManager::getTexture(const std::string id) const
{
	return getTextureStruct(id).texture;
}

const SDL_Rect& TextureManager::getTextureRect(const std::string id) const
{
	return getTextureStruct(id).textureRect;
}

void TextureManager::loadFromJsonObject(const nlohmann::json& assetsJson)
{
	if (assetsJson.find("assets") != assetsJson.end())
	{
		const auto assets = assetsJson["assets"];
		for (const auto& asset : assets)
		{
			const std::string assetName{ asset["name"] };
			const std::string assetPath{ asset["path"] };
			const SDL_Rect assetRect{ asset.value("x", 0), asset.value("y", 0), asset["w"], asset["h"] };

			textureMap[assetName] = { assetRect, loadTexture(assetPath, renderer) };
		}
	}
}

}