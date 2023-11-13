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
	BLUE,
	PURPLE,
	YELLOW
};

const static std::unordered_map<std::string, eChipColor> chipNameMap =
{
	{"red", eChipColor::RED},
	{"green", eChipColor::GREEN},
	{"blue", eChipColor::BLUE},
	{"purple", eChipColor::PURPLE},
	{"yellow", eChipColor::YELLOW}
};

class ChipBase
{
public:
	virtual ~ChipBase() = default;

	// Interface
	virtual std::string getColorName() const = 0;
	virtual bool hasColor() const = 0;
};

class BasicChip : public ChipBase
{
public:
	// Interface
	std::string getColorName() const override { return ""; }
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

	// Interface
	std::string getColorName() const override;
	bool hasColor() const override { return true; }

private:
	eChipColor color;
};

}