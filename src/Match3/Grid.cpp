#include "Grid.h"
#include "Chips.h"

#include <random>
#include <algorithm>

namespace match3
{

bool cellsAreNeighbors(int row1, int col1, int row2, int col2)
{
	return ((row1 == row2 && std::abs(col1 - col2) == 1) || (col1 == col2 && std::abs(row1 - row2) == 1));
}

void Grid::init(const constants::Constants& constants)
{
	chipGenerationConfig = constants.getChipConfig();
	gridSize = constants.getGridSize();

	std::vector<double> probabilities;
	for (const auto& colorRule : chipGenerationConfig.colors)
		probabilities.emplace_back(colorRule.probability);

	randomColorDistribution = std::discrete_distribution<>(probabilities.begin(), probabilities.end());
}

void Grid::reset()
{
	grid.clear();
	resetSelected();
	chipsToDestroy.clear();

	generateGrid();
}

void Grid::generateGrid()
{
	for (int i = 0; i < gridSize; ++i)
	{
		std::vector<std::unique_ptr<ChipBase>> gridRow;
		for (int j = 0; j < gridSize; ++j)
		{
			gridRow.emplace_back(std::move(generateRandomChip()));
		}
		grid.emplace_back(std::move(gridRow));
	}
	removeCombos();
}

void Grid::setSelectedCell(int row, int col)
{
	if (hasSelectedPair())
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
}

void Grid::resetSelected()
{
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

bool Grid::hasChipsToDestroy() const
{
	return !chipsToDestroy.empty();
}

void Grid::checkForCombo(int row, int col, std::string color, std::vector<std::pair<int, int>>& markedChips)
{
	int rowIt = row, colIt = col;
	std::vector<std::pair<int, int>> rowCombo;
	std::vector<std::pair<int, int>> colCombo;

	markedChips.emplace_back(rowIt, colIt);
	rowCombo.emplace_back(rowIt, colIt);
	colCombo.emplace_back(rowIt, colIt);

	bool hasFullLine = false;

	rowIt = row - 1;
	while (checkCellColor(rowIt, col, color)) rowCombo.emplace_back(rowIt--, col);

	rowIt = row + 1;
	while (checkCellColor(rowIt, col, color)) rowCombo.emplace_back(rowIt++, col);

	colIt = col - 1;
	while (checkCellColor(row, colIt, color)) colCombo.emplace_back(row, colIt--);

	colIt = col + 1;
	while (checkCellColor(row, colIt, color)) colCombo.emplace_back(row, colIt++);

	if (rowCombo.size() >= 3)
		markedChips.insert(markedChips.begin(), rowCombo.begin(), rowCombo.end());

	if (colCombo.size() >= 3)
		markedChips.insert(markedChips.begin(), colCombo.begin(), colCombo.end());
}

bool Grid::checkCellColor(int row, int col, std::string color)
{
	return row >= 0 && row < gridSize &&
		col >= 0 && col < gridSize &&
		grid[row][col] &&
		grid[row][col]->getColorName() == color;
}

std::unique_ptr<ChipBase> Grid::generateRandomChip(const std::vector<std::string>& bannedColors)
{
	int colorIndex;
	std::string chipColor;

	do
	{
		colorIndex = randomColorDistribution(gen);
		chipColor = chipGenerationConfig.colors[colorIndex].name;
	} while (std::find(bannedColors.begin(), bannedColors.end(), chipColor) != bannedColors.end());

	return std::make_unique<ColoredChipDecorator>(std::make_unique<BasicChip>(), chipColor);
}

void Grid::slideChipsDown()
{
	for (int col = 0; col < gridSize; ++col)
	{
		slideColumnDown(col);
		generateChipsInEmptyCells(col);
	}
}

std::vector<std::vector<std::pair<int, int>>> Grid::getChipsToSlide() const
{
	std::vector<std::vector<std::pair<int, int>>> chipsToSlide;
	chipsToSlide.assign(gridSize, std::vector<std::pair<int, int>>());

	for (int col = 0; col < gridSize; ++col)
	{
		int emptyCells = 0;
		for (int row = gridSize - 1; row >= 0; --row)
		{
			if (!grid[row][col])
			{
				++emptyCells;
				continue;
			}

			if (emptyCells > 0) chipsToSlide[col].emplace_back(row, row + emptyCells);
		}
	}

	const auto emptyCellsCountInCols = getEmptyCellsCountInCols();
	addGeneratedChipsToChipsToSlide(chipsToSlide, emptyCellsCountInCols);

	return chipsToSlide;
}

bool Grid::hasSelectedPair() const
{
	return selectedCells[0].first >= 0 && selectedCells[0].second >= 0 &&
		   selectedCells[1].first >= 0 && selectedCells[1].second >= 0;
}

std::vector<int> Grid::getEmptyCellsCountInCols() const
{
	std::vector<int> emptyCellsCountInCols(gridSize, 0);
	for (int row = 0; row < gridSize; ++row)
	{
		for (int col = 0; col < gridSize; ++col)
		{
			if (!grid[row][col]) ++emptyCellsCountInCols[col];
		}
	}
	return emptyCellsCountInCols;
}

void Grid::slideColumnDown(int col)
{
	int row = gridSize - 2;
	while (row >= 0)
	{
		if (!grid[row + 1][col] && grid[row][col])
		{
			int rowIt = row;
			while (rowIt < gridSize - 1 && !grid[rowIt + 1][col])
			{
				std::swap(grid[rowIt + 1][col], grid[rowIt][col]);
				++rowIt;
			}
		}
		--row;
	}
}

void Grid::addGeneratedChipsToChipsToSlide(std::vector<std::vector<std::pair<int, int>>>& chipsToSlide, const std::vector<int>& emptyCellCountInCols) const
{
	for (int col = 0; col < gridSize; ++col)
	{
		if (emptyCellCountInCols[col] == 0)
			continue;

		int emptyCellsInCol = emptyCellCountInCols[col];
		int newChipStartPos = -1;
		for (int i = 0; i < emptyCellsInCol; ++i)
		{
			chipsToSlide[col].emplace_back(newChipStartPos, newChipStartPos + emptyCellsInCol);
			--newChipStartPos;
		}
	}
}

void Grid::generateChipsInEmptyCells(int col)
{
	int row = 0;
	while (row < gridSize && !grid[row][col])
	{
		grid[row++][col] = generateRandomChip();
	}
}

void Grid::removeCombos()
{
	bool changed = false;

	do
	{
		changed = false;

		int verticalCombo = 1;
		for (int col = 0; col < gridSize; ++col)
		{
			for (int row = 1; row < gridSize; ++row)
			{
				if (grid[row][col]->getColorName() == grid[row - 1][col]->getColorName()) ++verticalCombo;
				else verticalCombo = 1;

				if (verticalCombo == 3)
				{
					grid[row][col] = generateRandomChip({ grid[row][col]->getColorName() });
					verticalCombo = 1;
					changed = true;
				}
			}
		}

		int horizontalCombo = 1;
		for (int row = 0; row < gridSize; ++row)
		{
			for (int col = 1; col < gridSize; ++col)
			{
				if (grid[row][col]->getColorName() == grid[row][col - 1]->getColorName()) ++horizontalCombo;
				else horizontalCombo = 1;

				if (horizontalCombo == 3)
				{
					grid[row][col] = generateRandomChip({ grid[row][col]->getColorName() });
					horizontalCombo = 1;
					changed = true;
				}
			}
		}
	} while (changed);
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