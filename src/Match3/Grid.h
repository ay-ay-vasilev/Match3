#pragma once
#include "Chips.h"
#include "Constants.h"

#include <vector>
#include <memory>
#include <random>

namespace match3
{

class Grid
{
public:

	// Interface
	void generate(const constants::Constants& constants)
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		const auto chipConfig = constants.getChipConfig();
		std::vector<double> probabilities;
		for (const auto& colorRule : chipConfig.colors)
			probabilities.emplace_back(colorRule.probability);

		std::discrete_distribution<> randomColorDistribution(probabilities.begin(), probabilities.end());

		const auto gridSize = constants.getGridSize();
		for (int i = 0; i < gridSize; ++i)
		{
			std::vector<std::unique_ptr<ChipBase>> gridRow;
			for (int j = 0; j < gridSize; ++j)
			{
				const int colorIndex = randomColorDistribution(gen);
				const std::string& chipColor = chipConfig.colors[colorIndex].name;

				std::unique_ptr<ChipBase> chip = std::make_unique<ColoredChipDecorator>(std::make_unique<BasicChip>(), chipColor);
				gridRow.emplace_back(std::move(chip));
			}
			grid.emplace_back(std::move(gridRow));
		}
	}

	const std::vector<std::vector<std::unique_ptr<ChipBase>>>& getGrid() const { return grid; }

private:
	std::vector<std::vector<std::unique_ptr<ChipBase>>> grid;
};

}