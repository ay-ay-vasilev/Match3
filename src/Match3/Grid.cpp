#include "Grid.h"
#include "Constants.h"
#include "Chips.h"

#include <random>

namespace match3
{

bool cellsAreNeighbors(int row1, int col1, int row2, int col2)
{
	return ((row1 == row2 && std::abs(col1 - col2) == 1) || (col1 == col2 && std::abs(row1 - row2) == 1));
}

void Grid::generate(const constants::Constants& constants)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const auto chipConfig = constants.getChipConfig();
	std::vector<double> probabilities;
	for (const auto& colorRule : chipConfig.colors)
		probabilities.emplace_back(colorRule.probability);

	std::discrete_distribution<> randomColorDistribution(probabilities.begin(), probabilities.end());

	gridSize = constants.getGridSize();
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

void Grid::setSelectedCell(int row, int col)
{
	if (selectedCells[0].first >= 0 && selectedCells[0].second >= 0 &&
		selectedCells[1].first >= 0 && selectedCells[1].second >= 0)
	{
		selectedCells = { {{-1, -1}, {-1, -1}} };
	}

	if (selectedCells[0].first < 0 && selectedCells[0].second < 0)
	{
		selectedCells[0] = { row, col };
		return;
	}

	if (cellsAreNeighbors(row, col, selectedCells[0].first, selectedCells[0].second))
	{
		selectedCells[1] = { row, col };
		return;
	}

	selectedCells[0] = { row, col };
}

void Grid::swapSelected()
{
	std::swap(grid[selectedCells[0].first][selectedCells[0].second], grid[selectedCells[1].first][selectedCells[1].second]);
	selectedCells = { {{-1, -1}, {-1, -1}} };
}


void Grid::findAndMarkCombo(int row, int col)
{
	const auto& targetColor = grid[row][col]->getColorName();
	std::vector<std::pair<int, int>> markedChips;

	checkForCombo(row, col, targetColor, markedChips);

	if (markedChips.size() >= 3)
	{
		for (const auto& chipPos : markedChips)
			chipsToDestroy.emplace_back(chipPos);
	}
}

void Grid::checkForCombo(int row, int col, std::string color, std::vector<std::pair<int, int>>& markedChips)
{
	int rowIt = row, colIt = col;

	markedChips.emplace_back(rowIt, colIt);

	rowIt = row - 1;
	while (checkCellColor(rowIt, col, color)) markedChips.emplace_back(rowIt--, col);

	rowIt = row + 1;
	while (checkCellColor(rowIt, col, color)) markedChips.emplace_back(rowIt++, col);

	colIt = col - 1;
	while (checkCellColor(row, colIt, color)) markedChips.emplace_back(row, colIt--);

	colIt = col + 1;
	while (checkCellColor(row, colIt, color)) markedChips.emplace_back(row, colIt++);
}

bool Grid::checkCellColor(int row, int col, std::string color)
{
	return row >= 0 && row < gridSize &&
		col >= 0 && col < gridSize &&
		grid[row][col] &&
		grid[row][col]->getColorName() == color;
}

const std::vector<std::pair<int, int>> Grid::destroyMarkedChips()
{
	for (const auto& chip : chipsToDestroy)
	{
		grid[chip.first][chip.second].reset();
	}
	// return and nullify chipsToDestroy
	return std::move(chipsToDestroy);
}

}