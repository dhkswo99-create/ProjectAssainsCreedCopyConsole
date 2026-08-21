#pragma once

#include <Actor/Actor.h>


// 땅 담당 액터 클래스
class Ground : public Craft::Actor
{
	TYPE_DECLARATIONS(Ground, Actor)

public:
	Ground(const Craft::Vector2& position);
};

