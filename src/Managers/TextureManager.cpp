#include "TextureManager.h"

#include <string>
#include <iostream>

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

void TextureManager::init(SDL_Renderer* renderer)
{
	this->renderer = renderer;

	textureMap["cellTexture"] = loadTexture("assets/images/blue_square.png", renderer);

	textureMap["chip_red"] = loadTexture("assets/images/chip_red.png", renderer);
	textureMap["chip_green"] = loadTexture("assets/images/chip_green.png", renderer);
	textureMap["chip_blue"] = loadTexture("assets/images/chip_blue.png", renderer);
}

SDL_Texture* TextureManager::getTexture(const std::string id) const
{
	auto it = textureMap.find(id);
	if (it != textureMap.end())
		return it->second;

	std::cout << "Error: texture not found (" + id + ").\n";
	return nullptr;
}

}