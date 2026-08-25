#include "Bresenham.h"
#include <cassert>
#include <cmath>
#include <iostream>

#define ANGLE 180/3.14

Bresenham::Bresenham(std::vector<std::vector<int>>& map)
	: map(map)
{
	if (map.empty())
	{
		return;
	}
	ROW = map.size();
	COL = map[0].size();
}

std::vector<Vector2> Bresenham::BresenhamFinder(const float distance, const Vector2& startPosition, const Vector2& destination)
{
	std::vector<Vector2> nullVector;
    if (!IsInRange(startPosition)
        || !IsInRange(destination))
    {
        return nullVector;
    }
    if (IsDestination(
        startPosition,
        destination))
    {
        return nullVector;
    }

	std::vector<Vector2> path;
	
    //직선의 방정식 생성
    float innerProduct_RightVector = static_cast<float>(
        (- startPosition.x + destination.x)
        );
    float absRightVector = 1;//(1,0)

    double bresenhamAngle = acos(innerProduct_RightVector / (distance)) * ANGLE;

	// 적용할 브레젠함 결정.
	Vector2 bresenhamFace = CalcAngle(startPosition, destination, bresenhamAngle);
	
	return CalcBresenham(startPosition, destination, bresenhamFace);

}

bool Bresenham::IsDestination(
    const Vector2& currentPositon,
    const Vector2& destination
)
{
    return (
        currentPositon.x == destination.x
        && currentPositon.y == destination.y
        );
}
bool Bresenham::IsInRange(const Vector2& currentPositon)
{
    return (
        currentPositon.x >= 0 && currentPositon.x < COL
        && currentPositon.y >= 0&& currentPositon.y < ROW
        );
}

Vector2 Bresenham::CalcAngle(const Vector2& startPosition, const Vector2& destination, double newAngle)
{
	double bresenhamAngle = newAngle;
	Vector2 facingBresenham(0, 0);
	if (startPosition.y - destination.y < 0)
	{
		if (bresenhamAngle < 45)
		{
			facingBresenham = Vector2(2, 1);
		}
		else if (bresenhamAngle < 90)
		{
			facingBresenham = Vector2(1, 2);
		}
		else if (bresenhamAngle < 135)
		{
			facingBresenham = Vector2(-1, 2);
		}
		else if (bresenhamAngle >= 135)
		{
			facingBresenham = Vector2(-2, 1);
		}
	}
	else
	{
		if (bresenhamAngle < 45)
		{
			facingBresenham = Vector2(2, -1);
		}
		else if (bresenhamAngle < 90)
		{
			facingBresenham = Vector2(1, -2);
		}
		else if (bresenhamAngle < 135)
		{
			facingBresenham = Vector2(-1, -2);
		}
		else if (bresenhamAngle >= 135)
		{
			facingBresenham = Vector2(-2, -1);
		}
	}
	return facingBresenham;
}

std::vector<Vector2> Bresenham::CalcBresenham(const Vector2& startPosition, const Vector2& destination, const Vector2 bresenhamFace)
{
	std::vector<Vector2> bresenhamPath;
	int dx = destination.x - startPosition.x;
	int dy = destination.y - startPosition.y;
	int calValue = bresenhamFace.x * 4 + bresenhamFace.y;
	Vector2 currentPosition = startPosition;
	Vector2 mainFace;
	Vector2 subFace;
	switch (calValue) //방향 설정
	{
	case -9:
		mainFace = Vector2(-1, 0);
		subFace = Vector2(0, -1);
		break;
	case -7:
		mainFace = Vector2(-1, 0);
		subFace = Vector2(0, 1);
		break;
	case -6:
		mainFace = Vector2(0, -1);
		subFace = Vector2(-1, 0);
		break;
	case -2:
		mainFace = Vector2(0, 1);
		subFace = Vector2(-1, 0);
		break;
	case 2:
		mainFace = Vector2(0, -1);
		subFace = Vector2(1, 0);
		break;
	case 6:
		mainFace = Vector2(0, 1);
		subFace = Vector2(1, 0);
		break;
	case 7:
		mainFace = Vector2(1, 0);
		subFace = Vector2(0, -1);
		break;
	case 9:
		mainFace = Vector2(1, 0);
		subFace = Vector2(0, 1);
		break;
	} //방향 나누기
	auto LineFunc = [dx, dy, startPosition, currentPosition](const Vector2& sub)
		{ // 직선으로부터 떨어진 거리 계산
			return dx * (sub.y - startPosition.y) - dy * (sub.x - startPosition.x);
		};
	while (currentPosition != destination)
	{
		currentPosition = currentPosition + mainFace; 
		currentPosition = ( // 더 가까운 점 선택
			std::abs( LineFunc(currentPosition + subFace) ) > std::abs( LineFunc(currentPosition) ) ) 
			? currentPosition : currentPosition + subFace; 
		bresenhamPath.emplace_back(currentPosition);
	} 
	bresenhamPath.pop_back(); // 목적지는 제외
	// 정식 때는 이거 주석 처리해서 화살이 플레이어까지 닿게 하고
	// Todo : 게임 레벨에서 벽 처리할 때 한 칸 덜 체크하게 해야함

	return bresenhamPath;
}
