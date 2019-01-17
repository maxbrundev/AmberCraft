#include "pch.h"
#include "AmberCraft/Application.h"

int main()
{
	RenderEngine::Systems::Application Renderer;
	Renderer.Setup();
	Renderer.Run();
}