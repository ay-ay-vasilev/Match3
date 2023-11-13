#pragma once
#include "Util.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

namespace match3
{
	namespace bonus
	{
		// Bonus functions
		static void bonusBomb(int row, int col, int gridSize, std::vector<std::pair<int, int>>& markedChips)
		{
			if (row > 0				&& !util::isCellAlreadyAdded(row - 1, col, markedChips)) markedChips.emplace_back(row - 1, col);
			if (row < gridSize - 1	&& !util::isCellAlreadyAdded(row + 1, col, markedChips)) markedChips.emplace_back(row + 1, col);
			if (col > 0				&& !util::isCellAlreadyAdded(row, col - 1, markedChips)) markedChips.emplace_back(row, col - 1);
			if (col < gridSize - 1	&& !util::isCellAlreadyAdded(row, col + 1, markedChips)) markedChips.emplace_back(row, col + 1);
		}

		const static std::unordered_map<std::string, std::function<void(int, int, int, std::vector<std::pair<int, int>>&)>> bonusMap =
		{
			{"bomb", bonusBomb}
		};
	}
}