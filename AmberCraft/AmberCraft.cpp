#include "AmberCraftPCH.h"

#include "AmberCraft/Core/Game.h"

int main()
{
	AmberCraft::Core::Game game;

	game.Initialize();
	game.Run();

	return 0;
}