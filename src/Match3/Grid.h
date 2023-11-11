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
		std::uniform_int_distribution<> randomColorDistribution(0, 2); // todo

		const auto gridSize = constants.getGridSize();
		for (int i = 0; i < gridSize; ++i)
		{
			std::vector<std::unique_ptr<ChipBase>> gridRow;
			for (int j = 0; j < gridSize; ++j)
			{
				const int color = randomColorDistribution(gen);
				std::string chipColor = "";

				switch (color)
				{
				case 0:
					chipColor = "red";
					break;
				case 1:
					chipColor = "green";
					break;
				case 2:
					chipColor = "blue";
					break;
				}

				std::unique_ptr<ChipBase> chip = std::make_unique<ColoredChipDecorator>(std::move(std::make_unique<BasicChip>()), chipColor);
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