#include "assignment3.hpp"

#include <iostream>
#include <string>
#include "primitives.hpp"
#include "SimpFile.hpp"
#include "SimpEngine.hpp"
#include "RenderingEngine.hpp"

void draw_rect(Client* client, int x1, int y1, int x2, int y2, unsigned int color)
{
	for (int x = x1; x<x2; x++)
	{
		for (int y = y1; y<y2; y++)
		{
			client->getDrawable()->setPixel(x, y, color);
		}
	}
}

void assignment3::doNextPage(Client * client, const std::string& fileName)
{
	Rect bg{ 0, 0, 750, 750 };
	draw_rect(client, bg.x, bg.y, bg.right(), bg.bottom(), 0xffffffff);

	Rect viewPort{ 50, 50, 650, 650 };
	draw_rect(client, viewPort.x, viewPort.y, viewPort.right(), viewPort.bottom(), 0x00000000);

	if (!fileName.empty())
	{
		SimpFile file(fileName);
		RenderEngine renderer{ viewPort, client->getDrawable(), Color{255, 255, 255} };
		SimpEngine simpEngine(renderer);
		simpEngine.runCommands(file.commands());
	}
	else
	{
		SimpFile file("test.simp");
		RenderEngine renderer{ viewPort, client->getDrawable(), Color{ 255, 255, 255 } };
		SimpEngine simpEngine(renderer);
		simpEngine.runCommands(file.commands());
	}

	client->getDrawable()->updateScreen();   // you must call this to make the display change.
}
