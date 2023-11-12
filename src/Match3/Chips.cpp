#include "Chips.h"

namespace match3
{

std::string ColoredChipDecorator::getColorName() const
{
	const auto it = std::find_if(chipNameMap.begin(), chipNameMap.end(), [this](const auto& pair) {return pair.second == this->color; });
	if (it != chipNameMap.end())
	{
		return it->first;
	}
	return chip->getColorName();
}

}