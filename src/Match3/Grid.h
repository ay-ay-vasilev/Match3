#pragma once
#include "Constants.h"
#include <vector>
#include <memory>
#include <array>
#include <string>
#include <random>

namespace match3
{

bool cellsAreNeighbors(int row1, int col1, int row2, int col2);

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
	bool hasValidSwaps();

	void findAndMarkCombo(int row, int col);
	bool hasChipsToDestroy() const;
	const std::vector<std::pair<int, int>> destroyMarkedChips();
	void slideChipsDown();
	std::vector<std::vector<std::pair<int, int>>> getChipsToSlide() const;

	bool hasSelectedPair() const;
	const std::array<std::pair<int, int>, 2> getSelectedCells() const { return selectedCells; }
	const std::vector<std::vector<std::unique_ptr<ChipBase>>>& getGrid() const { return grid; }

private:
	// Constants
	const int MATCH = 3;

	// Service
	void checkForCombo(int row, int col, std::string color, std::vector<std::pair<int, int>>& markedChips) const;
	bool checkCellColor(int row, int col, std::string color) const;
	bool checkIfSwapValid(int row1, int col1, int row2, int col2);
	std::unique_ptr<ChipBase> generateRandomChip(const std::vector<std::string>& bannedColors = {});
	std::vector<int> getEmptyCellsCountInCols() const;
	void slideColumnDown(int col);
	void addGeneratedChipsToChipsToSlide(std::vector<std::vector<std::pair<int, int>>>& chipsToSlide, const std::vector<int>& emptyCellCountInCols) const;
	void generateChipsInEmptyCells(int col);
	void removeCombos();

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