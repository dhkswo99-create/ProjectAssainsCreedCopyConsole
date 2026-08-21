#pragma once

#include <Actor/Actor.h>


// 벽 담당 액터 클래스
class Wall : public Craft::Actor
{
	TYPE_DECLARATIONS(Wall, Actor)

public:
	Wall(const Craft::Vector2& position);
};

