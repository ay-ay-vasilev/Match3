#pragma once
#include <vector>
#include <memory>
#include <array>
#include <string>

namespace constants
{
	class Constants;
}

namespace match3
{

bool cellsAreNeighbors(int row1, int col1, int row2, int col2);

class ChipBase;
class Grid
{
public:
	// Interface
	void generate(const constants::Constants& constants);
	void setSelectedCell(int row, int col);
	void swapSelected();

	void findAndMarkCombo(int row, int col);
	const std::vector<std::pair<int, int>> destroyMarkedChips();

	const std::array<std::pair<int, int>, 2> getSelectedCells() const { return selectedCells; }
	const std::vector<std::vector<std::unique_ptr<ChipBase>>>& getGrid() const { return grid; }

private:
	// Service
	void checkForCombo(int row, int col, std::string color, std::vector<std::pair<int, int>>& markedChips);
	bool checkCellColor(int row, int col, std::string color);

	// Members
	std::vector<std::vector<std::unique_ptr<ChipBase>>> grid;
	std::array<std::pair<int, int>, 2> selectedCells{ { {-1, -1}, {-1, -1} } };
	std::vector<std::pair<int, int>> chipsToDestroy;

	int gridSize;
};

}