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

enum class eBonus
{
	BOMB
};

const static std::unordered_map<std::string, eChipColor> chipNameMap =
{
	{"red", eChipColor::RED},
	{"green", eChipColor::GREEN},
	{"blue", eChipColor::BLUE},
	{"purple", eChipColor::PURPLE},
	{"yellow", eChipColor::YELLOW}
};

const static std::unordered_map<std::string, eBonus> bonusNameMap =
{
	{"bomb", eBonus::BOMB}
};

class ChipBase
{
public:
	virtual ~ChipBase() = default;

	// Interface
	virtual std::string getColorName() const = 0;
	virtual std::string getBonusName() const = 0;
	virtual bool hasColor() const = 0;
	virtual bool hasBonus() const = 0;
};

class BasicChip : public ChipBase
{
public:
	// Interface
	std::string getColorName() const override { return ""; }
	std::string getBonusName() const override { return ""; }
	bool hasColor() const override { return false; }
	bool hasBonus() const override { return false; }

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

	std::string getBonusName() const override { return chip->getBonusName(); }
	bool hasBonus() const override { return chip->hasBonus(); }

private:
	eChipColor color;
};

class BonusChipDecorator : public ChipDecorator
{
public:
	BonusChipDecorator(std::unique_ptr<ChipBase> chip, const std::string& bonus) : ChipDecorator(std::move(chip)), bonus(bonusNameMap.at(bonus)) {}

	// Interface
	std::string getBonusName() const override;
	bool hasBonus() const override { return true; }

	std::string getColorName() const override { return chip->getColorName(); }
	bool hasColor() const override { return chip->hasColor(); }

private:
	eBonus bonus;
};

}