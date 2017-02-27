#pragma once
#include <tuple>

typedef std::tuple<unsigned char, unsigned char, unsigned char> ColorChannels;

class Color
{
public:
	Color(unsigned char red, unsigned char green, unsigned char blue) : r(red), g(green), b(blue) {}
	
	Color(double color);

	Color(unsigned int color);

	unsigned int asUnsigned() const;

	ColorChannels getColorChannels() const;

	bool operator==(const Color& other) const;
private:
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

Color colorWithOpacity(const Color& newColor, const Color& oldColor, double opacity);