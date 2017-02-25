#include "client.h"

#include <iostream>
#include "assignment1.h"
#include "assignment2.h"
#include "SimpFile.h"
#include "Matrix.h"

Client::Client(Drawable *drawable)
{
    this->drawable = drawable;
}

void Client::nextPage() 
{
	Matrix<3, 2, int> matrix{ 1, 2, 3, 4, 5, 6 };
	auto row0 = matrix.getRow<1>();
	auto elem = matrix.getElement<1, 0>();
	auto elemFromRow = row0[0];
	auto col0 = matrix.getCol<1>();
	std::cout << elem;
	std::cout << elemFromRow;
	assignment2::doNextPage(this);
}

Drawable* Client::getDrawable() const
{
	return drawable;
}

void Client::draw_rect(int x1, int y1, int x2, int y2, unsigned int color) 
{
    for(int x = x1; x<x2; x++) 
	{
        for(int y=y1; y<y2; y++) 
		{
            drawable->setPixel(x, y, color);
        }
    }
}
