#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>

namespace match3
{

enum class eChipColor
{
	RED,
	GREEN,
	BLUE
};

const static std::unordered_map<std::string, eChipColor> chipNameMap =
{
	{"red", eChipColor::RED},
	{"green", eChipColor::GREEN},
	{"blue", eChipColor::BLUE}
};

class ChipBase
{
public:
	virtual ~ChipBase() = default;

	virtual const std::string& getColorName() const = 0;
	virtual bool hasColor() const = 0;
};

class BasicChip : public ChipBase
{
public:
	const std::string& getColorName() const override
	{
		return "";
	}

	bool hasColor() const override { return false; }

private:

};

class ChipDecorator : public ChipBase
{
public:
	ChipDecorator(std::unique_ptr<ChipBase> chip) : chip(std::move(chip)) {}
protected:
	std::unique_ptr<ChipBase> chip;
};

class ColoredChipDecorator : public ChipDecorator
{
public:
	ColoredChipDecorator(std::unique_ptr<ChipBase> chip, const std::string& color) : ChipDecorator(std::move(chip)), color(chipNameMap.at(color)) {}

	const std::string& getColorName() const override
	{
		const auto it = std::find_if(chipNameMap.begin(), chipNameMap.end(), [this](const auto& pair) {return pair.second == this->color; });
		if (it != chipNameMap.end())
		{
			return it->first;
		}
		return chip->getColorName();
	}

	bool hasColor() const override
	{
		return true;
	}

private:
	eChipColor color;
};

}