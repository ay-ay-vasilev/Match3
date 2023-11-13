#include "Grid.h"
#include "Chips.h"
#include "ChipBonusFunctions.h"
#include "Util.h"

#include <random>
#include <algorithm>

namespace match3
{

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
}

void Grid::generateGrid()
{
	do
	{
		reset();
		for (int i = 0; i < gridSize; ++i)
		{
			std::vector<std::unique_ptr<ChipBase>> gridRow;
			for (int j = 0; j < gridSize; ++j)
			{
				gridRow.emplace_back(std::move(generateRandomChip()));
			}
			grid.emplace_back(std::move(gridRow));
		}
		removeMatches();
	} while (!hasValidSwaps());

	generateRandomBonusChip();
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

	if (match3::util::areCellsNeighbors(row, col, selectedCells[0].first, selectedCells[0].second))
	{
		selectedCells[1] = { row, col };
		return;
	}

	selectedCells[0] = { row, col };
}

void Grid::swapCells(int row1, int col1, int row2, int col2)
{
	std::swap(grid[row1][col1], grid[row2][col2]);
}

void Grid::swapSelected()
{
	swapCells(selectedCells[0].first, selectedCells[0].second, selectedCells[1].first, selectedCells[1].second);
}

void Grid::resetSelected()
{
	selectedCells = { {{-1, -1}, {-1, -1}} };
}

bool Grid::hasValidSwaps()
{
	for (int row = 0; row < gridSize; ++row)
	{
		for (int col = 0; col < gridSize; ++col)
		{
			if (row > 0 && checkIfSwapValid(row, col, row - 1, col))
				return true;
			if (row < gridSize - 1 && checkIfSwapValid(row, col, row + 1, col))
				return true;
			if (col > 0 && checkIfSwapValid(row, col, row, col - 1))
				return true;
			if (col < gridSize - 1 && checkIfSwapValid(row, col, row, col + 1))
				return true;
		}
	}

	return false;
}

void Grid::findAndMarkMatches(int row, int col)
{
	const auto& targetColor = grid[row][col]->getColorName();
	std::vector<std::pair<int, int>> markedChips;

	checkForMatches(row, col, targetColor, markedChips);

	if (markedChips.size() < MATCH) return;

	for (const auto& chip : markedChips)
		if (!match3::util::isCellAlreadyAdded(chip.first, chip.second, chipsToDestroy)) chipsToDestroy.emplace_back(chip);

	bool areChipsAdded = false;
	do
	{
		areChipsAdded = tryAddChipsFromBonus(markedChips);
		for (const auto& chip : markedChips)
			if (!match3::util::isCellAlreadyAdded(chip.first, chip.second, chipsToDestroy)) chipsToDestroy.emplace_back(chip);
	} while (areChipsAdded);
}

bool Grid::hasChipsToDestroy() const
{
	return !chipsToDestroy.empty();
}

void Grid::checkForMatches(int row, int col, std::string color, std::vector<std::pair<int, int>>& markedChips) const
{
	int rowIt = row, colIt = col;
	std::vector<std::pair<int, int>> rowMatch;
	std::vector<std::pair<int, int>> colMatch;

	markedChips.emplace_back(rowIt, colIt);
	rowMatch.emplace_back(rowIt, colIt);
	colMatch.emplace_back(rowIt, colIt);

	rowIt = row - 1;
	while (doesCellColorMatch(rowIt, col, color)) rowMatch.emplace_back(rowIt--, col);

	rowIt = row + 1;
	while (doesCellColorMatch(rowIt, col, color)) rowMatch.emplace_back(rowIt++, col);

	colIt = col - 1;
	while (doesCellColorMatch(row, colIt, color)) colMatch.emplace_back(row, colIt--);

	colIt = col + 1;
	while (doesCellColorMatch(row, colIt, color)) colMatch.emplace_back(row, colIt++);

	if (rowMatch.size() >= MATCH)
		markedChips.insert(markedChips.begin(), rowMatch.begin(), rowMatch.end());

	if (colMatch.size() >= MATCH)
		markedChips.insert(markedChips.begin(), colMatch.begin(), colMatch.end());

	auto it = std::unique(markedChips.begin(), markedChips.end());
	markedChips.erase(it, markedChips.end());
}

bool Grid::tryAddChipsFromBonus(std::vector<std::pair<int, int>>& markedChips) const
{
	bool chipsAdded = false;
	for (const auto& chip : chipsToDestroy)
	{
		if (!doesCellHaveBonus(chip.first, chip.second)) continue;

		chipsAdded = chipsAdded || tryApplyMarkedChipsFromBonus(chip.first, chip.second, markedChips);
	}
	return chipsAdded;
}

bool Grid::doesCellColorMatch(int row, int col, std::string color) const
{
	return row >= 0 && row < gridSize &&
		col >= 0 && col < gridSize &&
		grid.at(row).at(col) &&
		grid.at(row).at(col)->getColorName() == color;
}

bool Grid::doesCellHaveBonus(int row, int col) const
{
	return grid.at(row).at(col)->hasBonus();
}

bool Grid::checkIfSwapValid(int row1, int col1, int row2, int col2)
{
	std::vector<std::pair<int, int>> markedChips;

	swapCells(row1, col1, row2, col2);

	checkForMatches(row1, col1, grid[row1][col1]->getColorName(), markedChips);
	checkForMatches(row2, col2, grid[row2][col2]->getColorName(), markedChips);

	swapCells(row1, col1, row2, col2);

	return markedChips.size() >= MATCH;
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

const std::vector<std::vector<std::pair<int, int>>> Grid::getChipsToSlide() const
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

void Grid::generateRandomBonusChip()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> rowDistribution(0, gridSize - 1);
	std::uniform_int_distribution<size_t> colDistribution(0, gridSize - 1);

	size_t randomRow = rowDistribution(gen);
	size_t randomCol = colDistribution(gen);

	const auto chipColor = grid[randomRow][randomCol]->getColorName();
	grid[randomRow][randomCol] = std::make_unique<BonusChipDecorator>(std::make_unique<ColoredChipDecorator>(std::make_unique<BasicChip>(), chipColor), "bomb");
}

void Grid::removeMatches()
{
	bool changed = false;

	do
	{
		changed = false;

		int verticalMatch = 1;
		for (int col = 0; col < gridSize; ++col)
		{
			for (int row = 1; row < gridSize; ++row)
			{
				if (grid[row][col]->getColorName() == grid[row - 1][col]->getColorName()) ++verticalMatch;
				else verticalMatch = 1;

				if (verticalMatch == MATCH)
				{
					grid[row][col] = generateRandomChip({ grid[row][col]->getColorName() });
					verticalMatch = 1;
					changed = true;
				}
			}
		}

		int horizontalMatch = 1;
		for (int row = 0; row < gridSize; ++row)
		{
			for (int col = 1; col < gridSize; ++col)
			{
				if (grid[row][col]->getColorName() == grid[row][col - 1]->getColorName()) ++horizontalMatch;
				else horizontalMatch = 1;

				if (horizontalMatch == MATCH)
				{
					grid[row][col] = generateRandomChip({ grid[row][col]->getColorName() });
					horizontalMatch = 1;
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

bool Grid::tryApplyMarkedChipsFromBonus(int row, int col, std::vector<std::pair<int, int>>& markedChips) const
{
	const auto markedChipsSize = markedChips.size();
	const auto bonusType = grid.at(row).at(col)->getBonusName();

	match3::bonus::bonusMap.at(bonusType)(row, col, gridSize, markedChips);

	return markedChipsSize < markedChips.size();
}

}