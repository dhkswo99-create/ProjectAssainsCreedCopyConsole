#pragma once

#define NOMINMAX

#include <vector>
#include <Math/Vector2.h>
#include <limits>

using namespace Craft;
struct CRAFT_API  Bresenham
{
public:
	Bresenham(std::vector<std::vector<int>>& map);
	~Bresenham() = default;

	std::vector<Vector2> BresenhamFinder(
		const Vector2& startPosition,
		const Vector2& destination
	);
	int GetROW() { return ROW; }
	int GetCOL() { return COL; }

private:
	bool IsDestination(const Vector2& currentPosition, const Vector2& destination);
	bool IsInRange(const Vector2& currentPosition);
	Vector2 CalcAngle(const Vector2& startPosition, const Vector2& destination, double newAngle);
	std::vector<Vector2> CalcBresenham(const Vector2& startPosition, const Vector2& destination, const Vector2 bresenhamFace);

private:
	//맵의 사이즈를 저장할 변수
	int ROW = 0;
	int COL = 0;
	//맵을 저장할 공간
	std::vector<std::vector<int>> map;
};