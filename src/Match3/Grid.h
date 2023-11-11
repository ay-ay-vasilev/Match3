#pragma once
#include "Chips.h"
#include "Constants.h"

#include <vector>
#include <memory>
#include <random>
#include <array>

namespace match3
{

bool cellsAreNeighbors(int row1, int col1, int row2, int col2)
{
	return ((row1 == row2 && std::abs(col1 - col2) == 1) || (col1 == col2 && std::abs(row1 - row2) == 1));
}

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

	void setSelectedCell(int row, int col)
	{
		if (selectedCells[0].first >= 0 && selectedCells[0].second >= 0 &&
			selectedCells[1].first >= 0 && selectedCells[1].second >= 0)
		{
			selectedCells = { {{-1, -1}, {-1, -1}} };
		}

		if (selectedCells[0].first < 0 && selectedCells[0].second < 0)
		{
			selectedCells[0] = {row, col};
			return;
		}

		if (cellsAreNeighbors(row, col, selectedCells[0].first, selectedCells[0].second))
		{
			selectedCells[1] = {row, col};
			return;
		}

		selectedCells[0] = {row, col};
	}

	const std::array<std::pair<int, int>, 2>& getSelectedCells() const
	{
		return selectedCells;
	}

	void swapSelected()
	{
		std::swap(grid[selectedCells[0].first][selectedCells[0].second], grid[selectedCells[1].first][selectedCells[1].second]);
		selectedCells = { {{-1, -1}, {-1, -1}} };
	}

	const std::vector<std::vector<std::unique_ptr<ChipBase>>>& getGrid() const { return grid; }

private:
	std::vector<std::vector<std::unique_ptr<ChipBase>>> grid;
	std::array<std::pair<int, int>, 2> selectedCells{ { {-1, -1}, {-1, -1} } };
};

}