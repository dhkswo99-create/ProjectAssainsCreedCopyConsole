#pragma once
#define NOMINMAX

#include <vector>
#include <Math/Vector2.h>
#include <limits>

using namespace Craft;
struct CRAFT_API  Astar
{
public:
	Astar(
		std::vector<std::vector<int>> map,
		const Vector2& startPosition,
		const Vector2& destination
	);
	~Astar() = default;

	std::vector<Vector2> AstarFinder(
		std::vector<std::vector<int>>& map,
		const Vector2& startPosition,
		const Vector2& destination
	);
	struct Node
	{
		int parentx, parenty;
		double h, f, g;
	};
	int GetROW() { return ROW; }
	int GetCOL() { return COL; }

private:
	bool IsDestination(int row, int col, const Vector2& destination);
	bool IsInRange(int row, int col);
	bool IsUnblocked(std::vector<std::vector<int>>& map, int row, int col);
	double CalcHValue(int row, int col, const Vector2& destination);
	std::vector<Vector2> TracePath(
		const std::vector<std::vector<Node>>& node,
		const Vector2& destination);

private:
	// 직선
	const int dx1[4] = { 0, 0, 1, -1 };
	const int dy1[4] = { -1, 1, 0, 0 };

	// 대각선
	const int dx2[4] = { 1, -1, -1, 1 };
	const int dy2[4] = { -1, 1, -1, 1 };
	//맵의 사이즈를 저장할 변수
	int ROW;
	int COL;
	//맵을 저장할 공간
	std::vector<std::vector<int>> map;
	std::vector<std::vector<char>> zmap;
	//무한
	double INF = std::numeric_limits<double>::infinity();
	const Vector2 startPosition;
	const Vector2 destination;
};