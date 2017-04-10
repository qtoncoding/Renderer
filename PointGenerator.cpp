#include "PointGenerator.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

#include "lerp.hpp"

namespace PointGenerator
{
	// Position helpers
	enum class Octant
	{
		First,
		Second,
		Third,
		Fourth,
		Fifth,
		Sixth,
		Seventh,
		Eighth
	};

	Point toFirstOctant(Octant octant, Point p)
	{
		switch (octant)
		{
		case Octant::First:
		{
			return Point{ p.x, p.y, p.z, p.parent, p.color };
		} break;

		case Octant::Second:
		{
			return Point{ p.y, p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Third:
		{
			return Point{ p.y, -p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Fourth:
		{
			return Point{ -p.x, p.y, p.z, p.parent, p.color };
		} break;

		case Octant::Fifth:
		{
			return Point{ -p.x, -p.y, p.z, p.parent, p.color };
		} break;

		case Octant::Sixth:
		{
			return Point{ -p.y, -p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Seventh:
		{
			return Point{ -p.y, p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Eighth:
		{
			return Point{ p.x, -p.y, p.z, p.parent, p.color };
		} break;

		default:
		{
			throw std::invalid_argument("Invalid octant");
		}
		}
	}

	Point fromFirstOctant(Octant octant, Point p)
	{
		switch (octant)
		{
		case Octant::First:
		{
			return Point{ p.x, p.y, p.z, p.parent, p.color };
		} break;

		case Octant::Second:
		{
			return Point{ p.y, p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Third:
		{
			return Point{ -p.y, p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Fourth:
		{
			return Point{ -p.x, p.y, p.z, p.parent, p.color };
		} break;

		case Octant::Fifth:
		{
			return Point{ -p.x, -p.y, p.z, p.parent, p.color };
		} break;

		case Octant::Sixth:
		{
			return Point{ -p.y, -p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Seventh:
		{
			return Point{ p.y, -p.x, p.z, p.parent, p.color };
		} break;

		case Octant::Eighth:
		{
			return Point{ p.x, -p.y, p.z, p.parent, p.color };
		} break;

		default:
		{
			throw std::invalid_argument("Invalid octant");
		}
		}
	}

	Octant getOctant(Point p)
	{
		auto aboveX = p.y > 0;
		auto rightY = p.x > 0;
		auto aboveDiag = std::abs(p.x) < std::abs(p.y);

		if (aboveX && rightY && !aboveDiag)
		{
			return Octant::First;
		}
		else if (aboveX && rightY && aboveDiag)
		{
			return Octant::Second;
		}
		else if (aboveX && !rightY && aboveDiag)
		{
			return Octant::Third;
		}
		else if (aboveX && !rightY && !aboveDiag)
		{
			return Octant::Fourth;
		}
		else if (!aboveX && !rightY && !aboveDiag)
		{
			return Octant::Fifth;
		}
		else if (!aboveX && !rightY && aboveDiag)
		{
			return Octant::Sixth;
		}
		else if (!aboveX && rightY && aboveDiag)
		{
			return Octant::Seventh;
		}
		else if (!aboveX && rightY && !aboveDiag)
		{
			return Octant::Eighth;
		}
		else
		{
			throw std::invalid_argument("Invalid octant");
		}
	}

	// Color helpers
	std::tuple<Lerp<double>, Lerp<double>, Lerp<double>> getColorLerp(const Point& point1, const Point& point2)
	{
		auto p1ColorChannels = point1.color.getColorChannels();
		auto p1Red = std::get<0>(p1ColorChannels);
		auto p1Green = std::get<1>(p1ColorChannels);
		auto p1Blue = std::get<2>(p1ColorChannels);

		auto p2ColorChannels = point2.color.getColorChannels();
		auto p2Red = std::get<0>(p2ColorChannels);
		auto p2Green = std::get<1>(p2ColorChannels);
		auto p2Blue = std::get<2>(p2ColorChannels);


		Lerp<double> redLerp(point1.x, point2.x, p1Red, p2Red);
		Lerp<double> greenLerp(point1.x, point2.x, p1Green, p2Green);
		Lerp<double> blueLerp(point1.x, point2.x, p1Blue, p2Blue);


		return std::make_tuple(redLerp, greenLerp, blueLerp);
	}

	Color getColorFromLerp(int colorIndex, const std::tuple<Lerp<double>, Lerp<double>, Lerp<double>>& colorLerps)
	{
		auto redLerp = std::get<0>(colorLerps);
		auto greenLerp = std::get<1>(colorLerps);
		auto blueLerp = std::get<2>(colorLerps);

		auto r = static_cast<unsigned char>(redLerp[colorIndex].second);
		auto g = static_cast<unsigned char>(greenLerp[colorIndex].second);
		auto b = static_cast<unsigned char>(blueLerp[colorIndex].second);

		return Color{ r, g, b };
	}

	std::vector<Point> generateLinePoints(const Point& p1, const Point & p2)
	{
		auto octant = getOctant(p2 - p1);
		auto point1 = toFirstOctant(octant, p1);
		auto point2 = toFirstOctant(octant, p2);

		auto colorLerps = getColorLerp(point1, point2);
		Lerp<decltype(point1.x)> posLerp(point1.x, point2.x, point1.y, point2.y);
		Lerp<decltype(point1.x)> zLerp(point1.x, point2.x, point1.z, point2.z);

		std::vector<Point> result;
		result.reserve(posLerp.size());
		auto currentIndex = 0;
		std::generate_n(std::back_inserter(result), 
						posLerp.size(),
						[&posLerp, &zLerp, &colorLerps, &currentIndex, octant]
						{
							auto point = posLerp[currentIndex];
							auto x = point.first;
							auto y = point.second;
							auto z = zLerp[currentIndex].second;
							auto newColor = getColorFromLerp(currentIndex, colorLerps);
							auto screenPoint = fromFirstOctant(octant, Point{ x, y, z, nullptr, newColor });
							++currentIndex;
							return screenPoint;
						});

		return result;
	}

	// Polygon helpers

	auto comparePoints = [](auto p1, auto p2)
	{
		if (p1.y == p2.y)
		{
			return p1.x < p2.x;
		}
		else
		{
			return p1.y < p2.y;
		}
	};

	double getXSlope(Line line)
	{
		// -
		if (line.p1.y == line.p2.y)
		{
			return 0.0;
		}
		else
		{
			// |
			if (line.p1.x == line.p2.x)
			{
				return 0.0;
			}
			else
			{
				return static_cast<double>(line.p2.x - line.p1.x) / static_cast<double>(line.p2.y - line.p1.y);
			}
		}
	}

	//template <typename T, typename F>
	//T sortVertices(const T& vertices, F comp)
	//{
	//	T sortedVertices{ vertices };
	//	std::sort(sortedVertices.begin(), sortedVertices.end(), comp);
	//	return sortedVertices;
	//}

	template <typename T>
	T getCenterPoint(const std::vector<T>& points)
	{
		auto centerPoint = std::accumulate(points.begin(), points.end(), T{ 0.0, 0.0, 0.0});
		centerPoint = centerPoint / static_cast<double>(points.size());
		return centerPoint;
	}

	template <typename T>
	T sortVertices(const T& vertices)
	{
		T sortedVertices{ vertices };
		auto center = getCenterPoint(vertices);
		std::sort(sortedVertices.begin(), sortedVertices.end(), [&center](const auto& a, const auto& b)
		{
			if (a.x - center.x >= 0 && b.x - center.x < 0)
				return false;
			if (a.x - center.x < 0 && b.x - center.x >= 0)
				return true;
			if (a.x - center.x == 0 && b.x - center.x == 0) {
				if (a.y - center.y >= 0 || b.y - center.y >= 0)
					return a.y < b.y;
				return b.y < a.y;
			}

			// compute the cross product of vectors (center -> a) x (center -> b)
			auto det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
			if (det < 0)
				return false;
			if (det > 0)
				return true;

			// points a and b are on the same line from the center
			// check which point is closer to the center
			auto d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
			auto d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
			return d1 < d2;
		});
		return sortedVertices;
	}


	template<typename T, typename F>
	std::tuple<T, T> splitVertices(const T& vertices, F sortComp)
	{
		std::vector<Point> leftChain = { vertices.front(), vertices.back() };
		std::vector<Point> rightChain = { vertices.front(), vertices.back() };

		auto centerX = 0.0;
		for (auto point : vertices)
		{
			centerX += point.x;
		}
		centerX /= static_cast<int>(vertices.size());

		auto endIt = std::prev(vertices.end());
		for (auto currentIt = std::next(vertices.begin()); currentIt != endIt; currentIt = std::next(currentIt))
		{
			auto currentPoint = *currentIt;
			if (currentPoint.x <= centerX)
			{
				leftChain.push_back(currentPoint);
			}
			else
			{
				rightChain.push_back(currentPoint);
			}
		}

		std::sort(leftChain.begin(), leftChain.end(), sortComp);
		std::sort(rightChain.begin(), rightChain.end(), sortComp);
		return std::make_tuple(leftChain, rightChain);
	}
	
	double edgeFunction(const Point& p1, const Point& p2, const Point& p)
	{
		return ((p.x - p1.x) * (p2.y - p1.y)) - ((p.y - p1.y) * (p2.x - p1.x));
	}

	std::vector<Point> generateTrianglePoints(const std::vector<Point>& vertices)
	{
		auto minX = std::floor((*std::min_element(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {return a.x < b.x; })).x);
		auto minY = std::floor((*std::min_element(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {return a.y < b.y; })).y);
		auto maxX = std::ceil((*std::max_element(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {return a.x < b.x; })).x);
		auto maxY = std::ceil((*std::max_element(vertices.begin(), vertices.end(), [](const auto& a, const auto& b) {return a.y < b.y; })).y);

		auto area = edgeFunction(vertices[0], vertices[1], vertices[2]);

		
		std::vector<Point> result;

		for (auto x = minX; x <= maxX; ++x)
		{
			for (auto y = minY; y <= maxY; ++y)
			{
				Point p{ x, y };
				auto w0= edgeFunction(vertices[0], vertices[1], p);
				auto w1= edgeFunction(vertices[1], vertices[2], p);
				auto w2= edgeFunction(vertices[2], vertices[0], p);

				if (w0 <= 0 && w1 <= 0 && w2 <= 0)
				{
					w0 /= area;
					w1 /= area;
					w2 /= area;
					
					auto z0 = 1 / vertices[0].z;
					auto z1 = 1 / vertices[1].z;
					auto z2 = 1 / vertices[2].z;

					auto oneOverZ = std::fma(z0, w0, std::fma(z1, w1, z2 * w2));
					auto z = 1.0 / oneOverZ;

					auto color = vertices[0].color * w0 + vertices[1].color * w1 + vertices[2].color * w2;
					result.push_back(Point{ x, y, z, nullptr, color });
				}
			}
		}

		return result;
	}

	std::vector<Point> generatePolygonPoints(const std::vector<Point>& points)
	{
		auto vertices = sortVertices(points);
		std::vector<Point> result;

		if (vertices.size() > 3)
		{
			std::vector<std::vector<Point>> triangles;
			for (auto i = 1u; i < vertices.size() - 1; ++i)
			{
				std::vector<Point> triangle;
				triangle.push_back(vertices[0]);
				triangle.push_back(vertices[i]);
				triangle.push_back(vertices[i + 1]);
				triangles.push_back(triangle);
			}

			for (auto& triangle : triangles)
			{
				auto trianglePoints = generateTrianglePoints(triangle);
				result.insert(result.end(), trianglePoints.begin(), trianglePoints.end());
			}

			return result;
		}
		else
		{
			return generateTrianglePoints(vertices);
		}
	}

	//std::vector<Point> generatePolygonPoints(const std::vector<Point>& points)
	//{
	//	//auto sortedVertices = sortVertices(points, comparePoints);
	//	auto sortedVertices = sortVertices(points);
	//	auto topPoint = sortedVertices.front();
	//	auto bottomPoint = sortedVertices.back();

	//	auto vertexChains = splitVertices(sortedVertices, comparePoints);

	//	// Left line chain
	//	auto leftChain = std::get<0>(vertexChains);
	//	auto leftIter = std::next(leftChain.begin());
	//	Line leftLine{ leftChain.front(), *leftIter };
	//	Lerp<double> leftLerp(leftLine.p1.y, leftLine.p2.y, leftLine.p1.x, leftLine.p2.x);
	//	Lerp<double> leftZLerp(leftLine.p1.y, leftLine.p2.y, leftLine.p1.z, leftLine.p2.z);

	//	auto leftColorLerps = getColorLerp(leftLine.p1.flipped(), leftLine.p2.flipped());
	//	auto leftRedLerp = std::get<0>(leftColorLerps);
	//	auto leftGreenLerp = std::get<1>(leftColorLerps);
	//	auto leftBlueLerp = std::get<2>(leftColorLerps);

	//	auto leftCount = 0;

	//	// Right line chain
	//	auto rightChain = std::get<1>(vertexChains);
	//	auto rightIter = std::next(rightChain.begin());
	//	Line rightLine{ rightChain.front(), *rightIter };
	//	Lerp<double> rightLerp(rightLine.p1.y, rightLine.p2.y, rightLine.p1.x, rightLine.p2.x);
	//	Lerp<double> rightZLerp(rightLine.p1.y, rightLine.p2.y, rightLine.p1.z, rightLine.p2.z);

	//	auto rightColorLerps = getColorLerp(rightLine.p1.flipped(), rightLine.p2.flipped());
	//	auto rightRedLerp = std::get<0>(rightColorLerps);
	//	auto rightGreenLerp = std::get<1>(rightColorLerps);
	//	auto rightBlueLerp = std::get<2>(rightColorLerps);

	//	auto rightCount = 0;

	//	std::vector<Point> result;

	//	for (auto y = static_cast<int>(topPoint.y); y <= bottomPoint.y; ++y)
	//	{
	//		auto xl = leftLerp[leftCount].second;
	//		auto zl = leftZLerp[leftCount].second;
	//		auto r = static_cast<unsigned char>(leftRedLerp[leftCount].second);
	//		auto g = static_cast<unsigned char>(leftGreenLerp[leftCount].second);
	//		auto b = static_cast<unsigned char>(leftBlueLerp[leftCount].second);
	//		++leftCount;
	//		auto leftColor = Color(r, g, b);

	//		auto xr = rightLerp[rightCount].second;
	//		auto zr = rightZLerp[rightCount].second;

	//		r = static_cast<unsigned char>(rightRedLerp[rightCount].second);
	//		g = static_cast<unsigned char>(rightGreenLerp[rightCount].second);
	//		b = static_cast<unsigned char>(rightBlueLerp[rightCount].second);
	//		++rightCount;
	//		auto rightColor = Color(r, g, b);

	//		auto leftPoint = Point{ xl, static_cast<double>(y), zl, topPoint.parent, leftColor };
	//		auto rightPoint = Point{ xr, static_cast<double>(y),  zr, topPoint.parent, rightColor };

	//		auto linePoints = generateLinePoints(leftPoint, rightPoint);
	//		result.insert(result.end(), linePoints.begin(), linePoints.end());

	//		if (y >= static_cast<int>(std::round(leftLine.p2.y)) && leftLine.p2 != bottomPoint)
	//		{
	//			leftIter = std::next(leftIter);
	//			leftLine = Line{ leftLine.p2, *leftIter };
	//			leftLerp = Lerp<double>(leftLine.p1.y, leftLine.p2.y, leftLine.p1.x, leftLine.p2.x);
	//			leftZLerp = Lerp<double>(leftLine.p1.y, leftLine.p2.y, leftLine.p1.z, leftLine.p2.z);

	//			leftColorLerps = getColorLerp(leftLine.p1.flipped(), leftLine.p2.flipped());
	//			leftRedLerp = std::get<0>(leftColorLerps);
	//			leftGreenLerp = std::get<1>(leftColorLerps);
	//			leftBlueLerp = std::get<2>(leftColorLerps);

	//			leftCount = 0;
	//		}

	//		if (y >= static_cast<int>(std::round(rightLine.p2.y)) && rightLine.p2 != bottomPoint)
	//		{
	//			rightIter = std::next(rightIter);
	//			rightLine = Line{ rightLine.p2, *rightIter };
	//			rightLerp = Lerp<double>(rightLine.p1.y, rightLine.p2.y, rightLine.p1.x, rightLine.p2.x);
	//			rightZLerp = Lerp<double>(rightLine.p1.y, rightLine.p2.y, rightLine.p1.z, rightLine.p2.z);


	//			rightColorLerps = getColorLerp(rightLine.p1.flipped(), rightLine.p2.flipped());
	//			rightRedLerp = std::get<0>(rightColorLerps);
	//			rightGreenLerp = std::get<1>(rightColorLerps);
	//			rightBlueLerp = std::get<2>(rightColorLerps);
	//			rightCount = 0;
	//		}
	//	}

	//	return result;
	//}

	std::vector<Point> generateWireframePoints(const std::vector<Point>& points)
	{
		//auto sortedVertices = sortVertices(points, comparePoints);
		auto sortedVertices = sortVertices(points);
		
		std::vector<Point> result;

		for (auto i = 0u; i < sortedVertices.size(); ++i)
		{
			auto j = i + 1;
			if (j == sortedVertices.size())
			{
				j = 0;
			}
			auto linePoints = generateLinePoints(sortedVertices[i], sortedVertices[j]);
			result.insert(result.end(), linePoints.begin(), linePoints.end());
		}

		return result;
	}
}
