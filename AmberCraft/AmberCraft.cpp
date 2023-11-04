#include "AmberCraftPCH.h"

#include "AmberCraft/Core/Game.h"

int main()
{
	uint64_t a = 5;
	float b = a;
	uint64_t c = b;
	AmberCraft::Core::Game game;

	game.Initialize();
	game.Run();

	return 0;
}