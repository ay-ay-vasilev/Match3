#pragma once

namespace events
{

struct ClickGameStateEvent
{
	int mouseX, mouseY;
};

struct ClickMatch3Event
{
	int mouseX, mouseY;
};

struct GridReadyEvent {};

struct PlayerTurnEvent {};
struct GridTurnEvent {};
struct RetryEvent {};

}