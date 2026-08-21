#include "Wall.h"

using namespace Craft;

Wall::Wall(const Vector2& position)
	: Actor(L"@", position,Color::White)
{
	//그리기 우선순위 지정
	sortingOrder = 2;
}
