#pragma once
#include <vector>

#include "drawable.h"
#include "primitives.hpp"
#include "CommonTypeAliases.hpp"
#include "Camera.hpp"

namespace PointsRenderer
{
	void renderPoints(const std::vector<Point>& points, Drawable* drawSurface, Matrix2D<double>& zBuffer, const Rect& viewPort, const Camera& camera);
}