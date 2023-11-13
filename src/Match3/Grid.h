#pragma once
#include "Constants.h"
#include <vector>
#include <memory>
#include <array>
#include <string>
#include <random>
#include <unordered_map>

namespace match3
{

class ChipBase;
class Grid
{
public:
	// Interface
	void init(const constants::Constants& constants);
	void reset();
	void generateGrid();
	void setSelectedCell(int row, int col);
	void swapCells(int row1, int col1, int row2, int col2);
	void swapSelected();
	void resetSelected();
	void findAndMarkMatches(int row, int col);
	void slideChipsDown();

	const std::vector<std::pair<int, int>> destroyMarkedChips();
	const std::vector<std::vector<std::pair<int, int>>> getChipsToSlide() const;

	bool hasValidSwaps();
	bool hasChipsToDestroy() const;
	bool hasSelectedPair() const;

	const std::array<std::pair<int, int>, 2> getSelectedCells() const { return selectedCells; }
	const std::vector<std::vector<std::unique_ptr<ChipBase>>>& getGrid() const { return grid; }

private:
	// Constants
	const int MATCH = 3;

	// Service
	void checkForMatches(int row, int col, std::string color, std::vector<std::pair<int, int>>& markedChips) const;
	void slideColumnDown(int col);
	void addGeneratedChipsToChipsToSlide(std::vector<std::vector<std::pair<int, int>>>& chipsToSlide, const std::vector<int>& emptyCellCountInCols) const;
	void generateChipsInEmptyCells(int col);
	void removeMatches();
	void generateRandomBonusChip();

	bool doesCellColorMatch(int row, int col, std::string color) const;
	bool doesCellHaveBonus(int row, int col) const;
	bool checkIfSwapValid(int row1, int col1, int row2, int col2);
	bool tryAddChipsFromBonus(std::vector<std::pair<int, int>>& markedChips) const;
	bool tryApplyMarkedChipsFromBonus(int row, int col, std::vector<std::pair<int, int>>& markedChips) const;

	std::unique_ptr<ChipBase> generateRandomChip(const std::vector<std::string>& bannedColors = {});
	std::vector<int> getEmptyCellsCountInCols() const;

	// Members
	std::vector<std::vector<std::unique_ptr<ChipBase>>> grid;
	std::array<std::pair<int, int>, 2> selectedCells{ { {-1, -1}, {-1, -1} } };
	std::vector<std::pair<int, int>> chipsToDestroy;

	// Parameters
	constants::ChipGenerationConfig chipGenerationConfig;
	int gridSize;

	// Random number generation
	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::discrete_distribution<> randomColorDistribution;
};

}