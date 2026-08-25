#include "Ground.h"

using namespace Craft;

Ground::Ground(const Vector2& position)
	: super(L" ", position) // super -> Actor 부모클래스 가리킴.
{
	//그리기 우선순위 지정
	//바닥 액터는 다른 액터랑 겹쳤을 때 덮어쓰기 되어야 함.
	bIsGround = true;
	isSighted = true;
	sortingOrder = 0;
}
