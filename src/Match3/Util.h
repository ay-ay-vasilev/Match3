#pragma once
#include <vector>

namespace match3
{
	namespace util
	{
		static bool areCellsNeighbors(int row1, int col1, int row2, int col2)
		{
			return ((row1 == row2 && std::abs(col1 - col2) == 1) || (col1 == col2 && std::abs(row1 - row2) == 1));
		}

		static bool isCellAlreadyAdded(int row, int col, const std::vector<std::pair<int, int>>& markedChips)
		{
			for (const auto& chip : markedChips) if (chip.first == row && chip.second == col) return true;
			return false;
		}
	}
}