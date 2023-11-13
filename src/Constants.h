#pragma once
#include <json.hpp>

#include <fstream>
#include <iostream>
#include <vector>

namespace constants
{

struct ChipGenerationRule
{
	std::string name;
	double probability;
};

struct ChipGenerationConfig
{
	std::vector<ChipGenerationRule> colors;
};

class Constants
{
public:

	void init(const std::string& fileName)
	{
		std::ifstream file(fileName);
		if (!file.is_open())
		{
			std::cerr << "Failed to open constants file: " << fileName << std::endl;
			return;
		}

		nlohmann::json constantsJson;
		file >> constantsJson;
		loadFromJsonObject(constantsJson);
	}

	// Interface
	const int getScreenWidth() const { return SCREEN_WIDTH; }
	const int getScreenHeight() const { return SCREEN_HEIGHT; }

	const int getGridSize() const { return GRID_SIZE; }
	const int getGridCellSize() const { return GRID_CELL_SIZE; }
	const float getScale() const { return SCALE; }

	const ChipGenerationConfig getChipConfig() const { return CHIP_CONFIG; }

private:
	// Service
	void loadFromJsonObject(const nlohmann::json& constantsJson)
	{
		if (constantsJson.find("screen_width") != constantsJson.end())
		{
			SCREEN_WIDTH = constantsJson["screen_width"];
		}
		if (constantsJson.find("screen_height") != constantsJson.end())
		{
			SCREEN_HEIGHT = constantsJson["screen_height"];
		}
		if (constantsJson.find("scale") != constantsJson.end())
		{
			SCALE = constantsJson["scale"];
		}
		if (constantsJson.find("grid_size") != constantsJson.end())
		{
			GRID_SIZE = constantsJson["grid_size"];
		}
		if (constantsJson.find("grid_cell_size") != constantsJson.end())
		{
			GRID_CELL_SIZE = constantsJson["grid_cell_size"];
		}
		if (constantsJson.find("chip_generation_rules") != constantsJson.end())
		{
			const auto colors = constantsJson["chip_generation_rules"]["colors"];
			for (const auto& color : colors)
			{
				CHIP_CONFIG.colors.emplace_back(color["name"], color["probability"]);
			}
		}
	}

	int SCREEN_WIDTH = 640;
	int SCREEN_HEIGHT = 640;
	float SCALE = 2.f;

	int GRID_SIZE = 10;
	int GRID_CELL_SIZE = 297;

	ChipGenerationConfig CHIP_CONFIG;
};

}