#pragma once

namespace constants
{

class Constants
{
public:

	const int getScreenWidth() const { return SCREEN_WIDTH; }
	const int getScreenHeight() const { return SCREEN_HEIGHT; }

	const int getGridSize() const { return GRID_SIZE; }
	const double getScale() const { return SCALE; }

private:

	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 640;
	const int GRID_SIZE = 10;
	const double SCALE = 2.f;

};

}